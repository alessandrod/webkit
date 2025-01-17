/*
 * Copyright (C) 2015 Canon Inc.
 * Copyright (C) 2015 Igalia S.L.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions
 * are required to be met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Canon Inc. nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY CANON INC. AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CANON INC. AND ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ReadableJSStream.h"

#if ENABLE(STREAMS_API)

#include "DOMWrapperWorld.h"
#include "JSDOMPromise.h"
#include "JSReadableStream.h"
#include "JSReadableStreamController.h"
#include "ScriptExecutionContext.h"
#include <runtime/Error.h>
#include <runtime/Exception.h>
#include <runtime/JSCJSValueInlines.h>
#include <runtime/JSPromise.h>
#include <runtime/JSString.h>
#include <runtime/StructureInlines.h>

using namespace JSC;

namespace WebCore {

static inline JSValue getPropertyFromObject(ExecState& exec, JSObject* object, const char* identifier)
{
    return object->get(&exec, Identifier::fromString(&exec, identifier));
}

static inline JSValue callFunction(ExecState& exec, JSValue jsFunction, JSValue thisValue, const ArgList& arguments)
{
    CallData callData;
    CallType callType = getCallData(jsFunction, callData);
    return call(&exec, jsFunction, callType, callData, thisValue, arguments);
}

JSPromise* ReadableJSStream::invoke(ExecState& state, const char* propertyName, JSValue parameter)
{
    JSValue function = getPropertyFromObject(state, m_source.get(), propertyName);
    if (state.hadException())
        return nullptr;

    if (!function.isFunction()) {
        if (!function.isUndefined())
            throwVMError(&state, createTypeError(&state, ASCIILiteral("ReadableStream trying to call a property that is not callable")));
        return nullptr;
    }

    MarkedArgumentBuffer arguments;
    arguments.append(parameter);

    JSPromise* promise = jsDynamicCast<JSPromise*>(callFunction(state, function, m_source.get(), arguments));

    ASSERT(!(promise && state.hadException()));
    return promise;
}

static void thenPromise(ExecState& state, JSPromise* deferredPromise, JSValue fullfilFunction, JSValue rejectFunction)
{
    JSValue thenValue = deferredPromise->get(&state, state.vm().propertyNames->then);
    if (state.hadException())
        return;

    MarkedArgumentBuffer arguments;
    arguments.append(fullfilFunction);
    arguments.append(rejectFunction);

    callFunction(state, thenValue, deferredPromise, arguments);
}

JSDOMGlobalObject* ReadableJSStream::globalObject()
{
    return jsDynamicCast<JSDOMGlobalObject*>(m_source->globalObject());
}

static inline JSFunction* createStartResultFulfilledFunction(ExecState& state, ReadableStream& readableStream)
{
    RefPtr<ReadableStream> stream = &readableStream;
    return JSFunction::create(state.vm(), state.callee()->globalObject(), 1, String(), [stream](ExecState*) {
        stream->start();
        return JSValue::encode(jsUndefined());
    });
}

static inline void startReadableStreamAsync(ReadableStream& stream)
{
    RefPtr<ReadableStream> protectedStream = &stream;
    stream.scriptExecutionContext()->postTask([protectedStream](ScriptExecutionContext&) {
        protectedStream->start();
    });
}

void ReadableJSStream::doStart(ExecState& exec)
{
    JSLockHolder lock(&exec);

    JSPromise* promise = invoke(exec, "start", jsController(exec, globalObject()));

    if (exec.hadException())
        return;

    if (!promise) {
        startReadableStreamAsync(*this);
        return;
    }

    thenPromise(exec, promise, createStartResultFulfilledFunction(exec, *this), m_errorFunction.get());
}

static inline JSFunction* createPullResultFulfilledFunction(ExecState& exec, ReadableJSStream& stream)
{
    RefPtr<ReadableJSStream> protectedStream = &stream;
    return JSFunction::create(exec.vm(), exec.callee()->globalObject(), 0, String(), [protectedStream](ExecState*) {
        protectedStream->finishPulling();
        return JSValue::encode(jsUndefined());
    });
}

bool ReadableJSStream::doPull()
{
    ExecState& state = *globalObject()->globalExec();
    JSLockHolder lock(&state);

    JSPromise* promise = invoke(state, "pull", jsController(state, globalObject()));

    if (promise)
        thenPromise(state, promise, createPullResultFulfilledFunction(state, *this), m_errorFunction.get());

    if (state.hadException()) {
        storeException(state);
        ASSERT(!state.hadException());
        return true;
    }

    return !promise;
}

static JSFunction* createCancelResultFulfilledFunction(ExecState& exec, ReadableJSStream& stream)
{
    RefPtr<ReadableJSStream> protectedStream = &stream;
    return JSFunction::create(exec.vm(), exec.callee()->globalObject(), 1, String(), [protectedStream](ExecState*) {
        protectedStream->notifyCancelSucceeded();
        return JSValue::encode(jsUndefined());
    });
}

static JSFunction* createCancelResultRejectedFunction(ExecState& exec, ReadableJSStream& stream)
{
    RefPtr<ReadableJSStream> protectedStream = &stream;
    return JSFunction::create(exec.vm(), exec.callee()->globalObject(), 1, String(), [protectedStream](ExecState* exec) {
        protectedStream->storeError(*exec, exec->argument(0));
        protectedStream->notifyCancelFailed();
        return JSValue::encode(jsUndefined());
    });
}

bool ReadableJSStream::doCancel(JSValue reason)
{
    ExecState& exec = *globalObject()->globalExec();
    JSLockHolder lock(&exec);

    JSPromise* promise = invoke(exec, "cancel", reason);

    if (promise)
        thenPromise(exec, promise, createCancelResultFulfilledFunction(exec, *this), createCancelResultRejectedFunction(exec, *this));

    if (exec.hadException()) {
        storeException(exec);
        ASSERT(!exec.hadException());
        return true;
    }
    return !promise;
}

RefPtr<ReadableJSStream> ReadableJSStream::create(ExecState& exec, ScriptExecutionContext& scriptExecutionContext)
{
    JSObject* jsSource;
    JSValue value = exec.argument(0);
    if (value.isObject())
        jsSource = value.getObject();
    else if (!value.isUndefined()) {
        throwVMError(&exec, createTypeError(&exec, ASCIILiteral("ReadableStream constructor first argument, if any, should be an object")));
        return nullptr;
    } else
        jsSource = JSFinalObject::create(exec.vm(), JSFinalObject::createStructure(exec.vm(), exec.callee()->globalObject(), jsNull(), 1));

    RefPtr<ReadableJSStream> readableStream = adoptRef(*new ReadableJSStream(scriptExecutionContext, exec, jsSource));
    readableStream->doStart(exec);

    if (exec.hadException())
        return nullptr;

    return readableStream;
}

ReadableJSStream::ReadableJSStream(ScriptExecutionContext& scriptExecutionContext, ExecState& state, JSObject* source)
    : ReadableStream(scriptExecutionContext)
{
    m_source.set(state.vm(), source);
    // We do not take a Ref to the stream as this would cause a Ref cycle.
    // The resolution callback used jointly with m_errorFunction as promise callbacks should protect the stream instead.
    m_errorFunction.set(state.vm(), JSFunction::create(state.vm(), state.callee()->globalObject(), 1, String(), [this](ExecState* state) {
        storeError(*state, state->argument(0));
        return JSValue::encode(jsUndefined());
    }));
}

JSValue ReadableJSStream::jsController(ExecState& exec, JSDOMGlobalObject* globalObject)
{
    if (!m_controller)
        m_controller = std::make_unique<ReadableStreamController>(*this);
    return toJS(&exec, globalObject, m_controller.get());
}

void ReadableJSStream::storeException(JSC::ExecState& state)
{
    JSValue exception = state.exception()->value();
    state.clearException();
    storeError(state, exception);
}

void ReadableJSStream::storeError(JSC::ExecState& exec, JSValue error)
{
    if (m_error)
        return;
    m_error.set(exec.vm(), error);

    changeStateToErrored();
}

bool ReadableJSStream::hasValue() const
{
    return m_chunkQueue.size();
}

JSValue ReadableJSStream::read()
{
    ASSERT(hasValue());

    return m_chunkQueue.takeFirst().get();
}

void ReadableJSStream::enqueue(ExecState& exec)
{
    ASSERT(!isCloseRequested());

    if (!isReadable())
        return;

    JSValue chunk = exec.argumentCount() ? exec.argument(0) : jsUndefined();
    if (resolveReadCallback(chunk)) {
        pull();
        return;
    }

    m_chunkQueue.append(JSC::Strong<JSC::Unknown>(exec.vm(), chunk));
    // FIXME: Compute chunk size.
    pull();
}

} // namespace WebCore

#endif
