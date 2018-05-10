#include <functional>
#include <memory>
#include <nosync/activity-owner.h>
#include <nosync/event-loop-utils.h>
#include <nosync/fd-watch-utils.h>
#include <nosync/result-utils.h>
#include <utility>

namespace ch = std::chrono;
using std::make_shared;
using std::move;


namespace nosync
{

namespace
{

struct timeouting_watch_context
{
    timeouting_watch_context() = default;

    void handle_result(result<void> res);

    result_handler<void> res_handler;
    activity_owner watch_owner;
    activity_owner timeout_task_owner;
};


void timeouting_watch_context::handle_result(result<void> res)
{
    watch_owner.reset();
    timeout_task_owner.reset();
    res_handler(res);
}

}


void watch_fd_with_timeout(
    fd_watching_event_loop &evloop, int fd, fd_watch_mode mode, ch::nanoseconds timeout,
    result_handler<void> &&res_handler)
{
    auto ctx = make_shared<timeouting_watch_context>();

    ctx->res_handler = move(res_handler);

    ctx->watch_owner = evloop.add_watch(
        fd, mode,
        [ctx]() {
            ctx->handle_result(make_ok_result());
        });
    ctx->timeout_task_owner = invoke_with_etime_delay(
        evloop, timeout,
        [ctx]() {
            ctx->handle_result(make_timeout_raw_error_result());
        });
}

}
