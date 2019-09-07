// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__DEADLINE_SETTING_REQUEST_HANDLER_IMPL_H
#define NOSYNC__DEADLINE_SETTING_REQUEST_HANDLER_IMPL_H

#include <algorithm>
#include <nosync/func-request-handler.h>
#include <utility>


namespace nosync
{

template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_deadline_setting_request_handler(
    event_loop &evloop, std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    eclock::time_point deadline)
{
    using std::move;

    return make_func_request_handler<Req, Res>(
        [&evloop, base_req_handler = move(base_req_handler), deadline](Req &&request, eclock::duration timeout, result_handler<Res> &&res_handler) {
            auto now = evloop.get_etime();
            auto eff_timeout = std::min(timeout, deadline >= now ? deadline - now : eclock::duration(0));
            base_req_handler->handle_request(move(request), eff_timeout, move(res_handler));
        });
}

}

#endif
