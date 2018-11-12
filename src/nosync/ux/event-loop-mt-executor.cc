// This file is part of libnosync library. See LICENSE file for license details.
#include <memory>
#include <mutex>
#include <nosync/activity-owner.h>
#include <nosync/event-loop-utils.h>
#include <nosync/raw-error-result.h>
#include <nosync/ux/event-loop-mt-executor.h>
#include <nosync/ux/fd-utils.h>
#include <nosync/ux/io-utils.h>
#include <queue>
#include <utility>

using std::enable_shared_from_this;
using std::function;
using std::make_shared;
using std::move;
using std::mutex;
using std::queue;
using std::scoped_lock;
using std::shared_ptr;
using std::string_view;


namespace nosync::ux
{

namespace
{

constexpr auto queue_fd_notify_byte = '\x01';
constexpr auto queue_fd_notify_read_size = 0x2000U;


template<typename T>
struct synchronized_queue
{
    queue<T> elements;
    mutex queue_mutex;
};


template<typename T>
class synchronized_queue_pusher
{
public:
    synchronized_queue_pusher(shared_ptr<synchronized_queue<function<void()>>> out_queue, owned_fd &&out_notify_fd);

    void push(T element);

private:
    shared_ptr<synchronized_queue<function<void()>>> out_queue;
    owned_fd out_notify_fd;
};


template<typename T>
synchronized_queue_pusher<T>::synchronized_queue_pusher(
    shared_ptr<synchronized_queue<function<void()>>> out_queue, owned_fd &&out_notify_fd)
    : out_queue(move(out_queue)), out_notify_fd(move(out_notify_fd))
{
}


template<typename T>
void synchronized_queue_pusher<T>::push(T element)
{
    scoped_lock queue_lock(out_queue->queue_mutex);
    out_queue->elements.push(move(element));
    if (out_queue->elements.size() == 1) {
        write_some_bytes_to_fd(*out_notify_fd, string_view(&queue_fd_notify_byte, 1));
    }
}


class queued_tasks_dispatcher : public enable_shared_from_this<queued_tasks_dispatcher>
{
public:
    queued_tasks_dispatcher(
        fd_watching_event_loop &evloop, shared_ptr<synchronized_queue<function<void()>>> tasks_queue,
        owned_fd &&in_notify_fd);

    void start();
    void handle_in_notify();

private:
    fd_watching_event_loop &evloop;
    shared_ptr<synchronized_queue<function<void()>>> tasks_queue;
    owned_fd in_notify_fd;
    activity_owner in_notify_watch_owner;
};


queued_tasks_dispatcher::queued_tasks_dispatcher(
    fd_watching_event_loop &evloop, shared_ptr<synchronized_queue<function<void()>>> tasks_queue,
    owned_fd &&in_notify_fd)
    : evloop(evloop), tasks_queue(move(tasks_queue)), in_notify_fd(move(in_notify_fd)),
    in_notify_watch_owner()
{
}


void queued_tasks_dispatcher::start()
{
    in_notify_watch_owner = evloop.add_watch(
        *in_notify_fd, fd_watch_mode::input,
        [dispatcher_ptr = shared_from_this()]() {
            dispatcher_ptr->handle_in_notify();
        });
}


void queued_tasks_dispatcher::handle_in_notify()
{
    auto notify_read_res = read_some_bytes_from_fd(*in_notify_fd, queue_fd_notify_read_size);

    {
        scoped_lock tasks_queue_lock(tasks_queue->queue_mutex);
        while (!tasks_queue->elements.empty()) {
            invoke_later(evloop, move(tasks_queue->elements.front()));
            tasks_queue->elements.pop();
        }
    }

    if (!notify_read_res.is_ok() || notify_read_res.get_value().empty()) {
        in_notify_watch_owner.reset();
    }
}

}


result<function<void(function<void()>)>> make_event_loop_mt_executor(fd_watching_event_loop &evloop)
{
    auto pipe_fds_res = open_pipe();
    if (!pipe_fds_res.is_ok()) {
        return raw_error_result(pipe_fds_res);
    }

    auto &pipe_fds = pipe_fds_res.get_value();

    auto tasks_queue = make_shared<synchronized_queue<function<void()>>>();

    auto dispatcher = make_shared<queued_tasks_dispatcher>(evloop, tasks_queue, move(pipe_fds[0]));
    dispatcher->start();

    auto tasks_pusher = make_shared<synchronized_queue_pusher<function<void()>>>(move(tasks_queue), move(pipe_fds[1]));
    return make_ok_result(
        [tasks_pusher = move(tasks_pusher)](function<void()> task) {
            tasks_pusher->push(move(task));
        });
}

}
