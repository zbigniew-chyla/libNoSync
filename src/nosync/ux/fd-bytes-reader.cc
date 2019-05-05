// This file is part of libnosync library. See LICENSE file for license details.
#include <nosync/activity-owner.h>
#include <nosync/result-handler-utils.h>
#include <nosync/result-utils.h>
#include <nosync/time-utils.h>
#include <nosync/type-utils.h>
#include <nosync/ux/fd-bytes-reader.h>
#include <nosync/ux/fd-utils.h>
#include <utility>

using std::make_shared;
using std::move;
using std::shared_ptr;
using std::size_t;
using std::string;


namespace nosync::ux
{

namespace
{

struct async_read_context
{
    async_read_context(shared_fd &&fd, size_t max_size, result_handler<string> &&res_handler);

    void handle_result(result<string> res);

    shared_fd fd;
    size_t max_size;
    result_handler<string> res_handler;
    activity_owner timeout_task_owner;
    activity_owner read_watch_owner;
};


class fd_bytes_reader : public bytes_reader
{
public:
    fd_bytes_reader(fd_watching_event_loop &evloop, shared_fd &&fd);

    void read_some_bytes(
        size_t max_size, eclock::duration timeout,
        result_handler<string> &&res_handler) override;

private:
    fd_watching_event_loop &evloop;
    shared_fd fd;
};


async_read_context::async_read_context(
    shared_fd &&fd, size_t max_size, result_handler<string> &&res_handler)
    : fd(move(fd)), max_size(max_size), res_handler(move(res_handler)),
    timeout_task_owner(), read_watch_owner()
{
}


void async_read_context::handle_result(result<string> res)
{
    timeout_task_owner.reset();
    read_watch_owner.reset();
    if (res_handler) {
        res_handler(move(res));
        res_handler = nullptr;
    }
}


fd_bytes_reader::fd_bytes_reader(fd_watching_event_loop &evloop, shared_fd &&fd)
    : evloop(evloop), fd(move(fd))
{
}


void fd_bytes_reader::read_some_bytes(
    size_t max_size, eclock::duration timeout,
    result_handler<string> &&res_handler)
{
    if (max_size == 0) {
        invoke_result_handler_later(evloop, move(res_handler), make_ok_result(string()));
        return;
    }

    auto ctx = make_shared<async_read_context>(make_copy(fd), max_size, move(res_handler));
    ctx->read_watch_owner = evloop.add_watch(
        *fd, fd_watch_mode::input,
        [ctx]() {
            ctx->handle_result(read_some_bytes_from_fd(*ctx->fd, ctx->max_size));
        });
    ctx->timeout_task_owner = evloop.invoke_at(
        time_point_sat_add(evloop.get_etime(), timeout),
        [ctx]() {
            ctx->handle_result(make_timeout_raw_error_result());
        });
}

}


shared_ptr<bytes_reader> make_fd_bytes_reader(fd_watching_event_loop &evloop, shared_fd &&fd)
{
    return make_shared<fd_bytes_reader>(evloop, move(fd));
}

}
