// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__IDLE_NOTIFYING_REQUEST_HANDLER_IMPL_H
#define NOSYNC__IDLE_NOTIFYING_REQUEST_HANDLER_IMPL_H

#include <nosync/func-request-handler.h>
#include <nosync/memory-utils.h>
#include <utility>


namespace nosync
{

template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_idle_notifying_request_handler(
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    std::function<void(bool)> &&idle_change_listener)
{
    using std::move;

    struct context
    {
        std::shared_ptr<request_handler<Req, Res>> base_req_handler;
        std::function<void(bool)> idle_change_listener;
        std::size_t active_reqs_counter;
    };

    auto ctx = move_to_shared(context{move(base_req_handler), move(idle_change_listener), 0U});

    return make_func_request_handler<Req, Res>(
        [ctx = move(ctx)](Req &&req, eclock::duration timeout, result_handler<Res> &&res_handler) {
            ++ctx->active_reqs_counter;
            if (ctx->active_reqs_counter == 1) {
                ctx->idle_change_listener(false);
            }
            ctx->base_req_handler->handle_request(
                move(req), timeout,
                [ctx, res_handler = move(res_handler)](auto res) {
                    res_handler(move(res));
                    --ctx->active_reqs_counter;
                    if (ctx->active_reqs_counter == 0) {
                        ctx->idle_change_listener(true);
                    }
                });
        });
}


template<typename Req, typename Res>
std::shared_ptr<request_handler<Req, Res>> make_idle_notifying_request_handler(
    std::shared_ptr<request_handler<Req, Res>> &&base_req_handler,
    std::function<void()> &&idle_listener)
{
    using std::move;

    return make_idle_notifying_request_handler(
        move(base_req_handler),
        [idle_listener = move(idle_listener)](auto idle) {
            if (idle) {
                idle_listener();
            }
        });
}

}

#endif
