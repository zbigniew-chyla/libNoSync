// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__CONST_RESPONSE_REQUEST_HANDLER_IMPL_H
#define NOSYNC__CONST_RESPONSE_REQUEST_HANDLER_IMPL_H

#include <nosync/func-request-handler.h>
#include <nosync/result-handler-utils.h>
#include <utility>


namespace nosync
{

template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_const_response_request_handler(
    event_loop &evloop, result<Res> &&response)
{
    return make_func_request_handler<Req, Res>(
        [&evloop, response = std::move(response)](Req &&, eclock::duration, result_handler<Res> &&res_handler) {
            invoke_result_handler_later(evloop, std::move(res_handler), response);
        });
}

}

#endif
