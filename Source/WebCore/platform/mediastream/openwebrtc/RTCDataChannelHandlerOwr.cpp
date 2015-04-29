/*
 * Copyright (C) 2015 Ericsson AB. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Ericsson nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(MEDIA_STREAM)
#include "RTCDataChannelHandlerOwr.h"
#include "MediaEndpoint.h"
#include <wtf/text/CString.h>

namespace WebCore {


static std::unique_ptr<RTCDataChannelHandler> createRTCDataChannelHandlerOwr( const String& label, RTCDataChannelInit_Endpoint& initData, OwrDataChannel* channel)
{
    return std::unique_ptr<RTCDataChannelHandler>(new RTCDataChannelHandlerOwr(label, initData, channel));
}

CreateRTCDataChannelHandler RTCDataChannelHandler::create = createRTCDataChannelHandlerOwr;

RTCDataChannelHandlerOwr::RTCDataChannelHandlerOwr(const String& label, RTCDataChannelInit_Endpoint& initData, OwrDataChannel* channel)
    : m_label(label)
    , m_initData(initData)
    , m_owrDataChannel(channel)
{
    
}

RTCDataChannelHandlerOwr::~RTCDataChannelHandlerOwr()
{
}

void RTCDataChannelHandlerOwr::setClient(RTCDataChannelHandlerClient* client){
    m_client = client;
};

String RTCDataChannelHandlerOwr::label(){
    return m_label;
};

bool RTCDataChannelHandlerOwr::ordered(){
    return m_initData.ordered ;
};

unsigned short RTCDataChannelHandlerOwr::maxRetransmitTime(){
    return m_initData.maxRetransmitTime;
};

unsigned short RTCDataChannelHandlerOwr::maxRetransmits(){
    return m_initData.maxRetransmits;
};

String RTCDataChannelHandlerOwr::protocol(){
    return m_initData.protocol;
};

bool RTCDataChannelHandlerOwr::negotiated(){
    return m_initData.negotiated;
};

unsigned short RTCDataChannelHandlerOwr::id(){
    return m_initData.id;
};

unsigned long RTCDataChannelHandlerOwr::bufferedAmount(){
    return m_bufferedAmount;    
};


bool RTCDataChannelHandlerOwr::sendStringData(const String&)
{
    
};

bool RTCDataChannelHandlerOwr::sendRawData(const char*, size_t){

    
};

void RTCDataChannelHandlerOwr::close(){
    
};


} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)
