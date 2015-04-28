/*
 *
 */

#include "config.h"

#if ENABLE(MEDIA_STREAM)
#include "RTCDataChannelHandler.h"

namespace WebCore {

static std::unique_ptr<RTCDataChannelHandler> createRTCDataChannelHandler(RTCDataChannelHandlerClient*, String label)
{
    return nullptr;
}

CreateRTCDataChannelHandler RTCDataChannelHandler::create = createRTCDataChannelHandler;

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)
