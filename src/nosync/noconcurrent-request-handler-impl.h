// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__NOCONCURRENT_REQUEST_HANDLER_IMPL_H
#define NOSYNC__NOCONCURRENT_REQUEST_HANDLER_IMPL_H

#include <nosync/event-loop.h>
#include <nosync/func-request-handler.h>
#include <nosync/request-handler.h>
#include <memory>
#include <system_error>
#include <utility>


namespace nosync
{

template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_noconcurrent_request_handler(
    event_loop &evloop, std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    std::error_code concurrent_calls_error)
{
    using std::move;

    auto call_active = std::make_shared<bool>(false);

    return make_func_request_handler<Req, Res>(
        [&evloop, base_req_handler = move(base_req_handler), concurrent_calls_error, call_active = move(call_active)](auto &&req, auto timeout, auto &&res_handler) {
            if (!*call_active) {
                *call_active = true;
                base_req_handler->handle_request(
                    move(req), timeout,
                    [call_active, res_handler = move(res_handler)](auto res) {
                        *call_active = false;
                        res_handler(move(res));
                    });
            } else {
                invoke_result_handler_later(
                    evloop, move(res_handler), raw_error_result(concurrent_calls_error));
            }
        });
}

}

#endif
