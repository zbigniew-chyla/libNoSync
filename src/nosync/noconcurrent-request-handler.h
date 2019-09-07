// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__NOCONCURRENT_REQUEST_HANDLER_H
#define NOSYNC__NOCONCURRENT_REQUEST_HANDLER_H

#include <nosync/event-loop.h>
#include <nosync/request-handler.h>
#include <memory>
#include <system_error>


namespace nosync
{

template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_noconcurrent_request_handler(
    event_loop &evloop, std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    std::error_code concurrent_calls_error = std::make_error_code(std::errc::operation_in_progress));

}

#include <nosync/noconcurrent-request-handler-impl.h>

#endif
