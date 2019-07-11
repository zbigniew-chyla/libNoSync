// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__LAZY_INIT_REQUEST_HANDLER_H
#define NOSYNC__LAZY_INIT_REQUEST_HANDLER_H

#include <cstddef>
#include <functional>
#include <memory>
#include <nosync/event-loop.h>
#include <nosync/request-handler.h>


namespace nosync
{

/*!
Create request_handler delegating all calls to request_handler created lazily (on first call).

The underlying request_handler<> is created using provided factory (on first request received by
the lazy request handler). The factory is used only once and then destroyed. The request sent to
the factory has timeout set to the maximum value (if other timeout is desired, the factory should
be decorated with the appropriate "timeout limiter").
*/

template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_lazy_init_request_handler(
    event_loop &evloop,
    std::shared_ptr<request_handler<std::nullptr_t, std::shared_ptr<request_handler<Req, Res>>>> &&base_req_handler_factory);

}

#include <nosync/lazy-init-request-handler-impl.h>

#endif /* NOSYNC__LAZY_INIT_REQUEST_HANDLER_H */
