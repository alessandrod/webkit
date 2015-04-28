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
#include "RTCDataChannelOwr.h"
#include "MediaEndPoint.h"
#include <wtf/text/CString.h>

namespace WebCore {


static std::unique_ptr<RTCDataChannelHandler> createRTCDataChannelHandleOwr(RTCDataChannelHandlerClient* client, const String& label, RTCDataChannelInit_Endpoint& initData, OwrDataChannel* channel)
{
    return std::unique_ptr<RTCDataChannelHandler>(new RTCDataChannelHandleOwr(client, label, initData, channel));
}

CreateRTCDataChannelHandler RTCDataChannelHandler::create = createRTCDataChannelHandleOwr;

RTCDataChannelHandleOwr::RTCDataChannelHandleOwr(RTCDataChannelHandlerClient* client, const String& label, RTCDataChannelInit_Endpoint& initData, OwrDataChannel* channel)
    : d_client(client)
    , label(label)
    , ordered(initData.ordered)
    , maxRetransmitTime(initData.maxRetransmitTime)
    , maxRetransmits(initData.maxRetransmits)
    , negotiated(initData.negotiated)
    , id(initData.id)
    , owrDataChannel(channel)
{
    
}

RTCDataChannelHandleOwr::~RTCDataChannelHandleOwr()
{
}

void RTCDataChannelHandleOwr::setClient(RTCDataChannelHandlerClient* client){
    d_client = client;
};

String RTCDataChannelHandleOwr::label(){
    return label;
};

bool RTCDataChannelHandleOwr::ordered(){
    return initData.ordered ;
};

unsigned short RTCDataChannelHandleOwr::maxRetransmitTime(){
    return initData.maxRetransmitTime; 
};

unsigned short RTCDataChannelHandleOwr::maxRetransmits(){
    return initData.maxRetransmits;
};

String RTCDataChannelHandleOwr::protocol(){
    return initData.protocol;
};

bool RTCDataChannelHandleOwr::negotiated(){
    return initData.negotiated; 
};

unsigned short RTCDataChannelHandleOwr::id(){
    return initData.id; 
};

unsigned long RTCDataChannelHandleOwr::bufferedAmount(){
    return bufferedAmount;    
};

OwrDataChannel RTCDataChannelHandleOwr::owrDataChannel(){
    return owrDataChannel;
};

bool RTCDataChannelHandleOwr::sendStringData(const String&){

    
};

bool RTCDataChannelHandleOwr::sendRawData(const char*, size_t){

    
};

void RTCDataChannelHandleOwr::close(){
    
};


} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)
