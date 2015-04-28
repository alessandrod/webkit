/*
 *
 */

#include "config.h"

#if ENABLE(MEDIA_STREAM)
#include "RTCDataChannelHandler.h"

namespace WebCore {

static std::unique_ptr<RTCDataChannelHandler> createRTCDataChannelHandler(RTCDataChannelHandlerClient*, const String& label, RTCDataChannelInit_Endpoint& initData, OwrDataChannel* channel)
{
    return nullptr;
}

CreateRTCDataChannelHandler RTCDataChannelHandler::create = createRTCDataChannelHandler;

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)
