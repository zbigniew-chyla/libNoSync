// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__TIMEOUT_LIMITING_REQUEST_HANDLER_H
#define NOSYNC__TIMEOUT_LIMITING_REQUEST_HANDLER_H

#include <memory>
#include <nosync/request-handler.h>


namespace nosync
{

/*!
Decorate request_handler<> limiting effective timeout to specified maximum value.
*/
template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_timeout_limiting_request_handler(
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    eclock::duration max_timeout);

}

#include <nosync/timeout-limiting-request-handler-impl.h>

#endif
