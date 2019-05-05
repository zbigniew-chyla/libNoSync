// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__TIMEOUT_LIMITING_REQUEST_HANDLER_IMPL_H
#define NOSYNC__TIMEOUT_LIMITING_REQUEST_HANDLER_IMPL_H

#include <algorithm>
#include <nosync/func-request-handler.h>
#include <utility>


namespace nosync
{

template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_timeout_limiting_request_handler(
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    eclock::duration max_timeout)
{
    using std::move;

    return make_func_request_handler<Req, Res>(
        [base_req_handler = move(base_req_handler), max_timeout](Req &&request, eclock::duration timeout, result_handler<Res> &&res_handler) {
            base_req_handler->handle_request(move(request), std::min(timeout, max_timeout), move(res_handler));
        });
}

}

#endif /* NOSYNC__TIMEOUT_LIMITING_REQUEST_HANDLER_IMPL_H */
