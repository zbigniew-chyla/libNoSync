#include <algorithm>
#include <iterator>
#include <nosync/exceptions.h>
#include <nosync/manual-event-loop.h>
#include <nosync/os/synchronized-queue.h>
#include <nosync/os/synchronized-queue-based-event-loop.h>
#include <optional>
#include <system_error>
#include <thread>
#include <utility>

using std::deque;
using std::errc;
using std::error_code;
using std::function;
using std::make_shared;
using std::make_unique;
using std::move;
using std::move_iterator;
using std::optional;
using std::shared_ptr;
using std::this_thread::sleep_for;
using std::unique_ptr;
using std::weak_ptr;


namespace nosync::os
{

namespace
{

deque<function<void()>> acquire_ext_tasks_with_abs_timeout(
    const shared_ptr<synchronized_queue<function<void()>>> &ext_tasks_queue,
    const eclock &clock, optional<eclock::time_point> abs_timeout)
{
    deque<function<void()>> tasks;
    if (abs_timeout.has_value()) {
        auto now = clock.now();
        auto pop_timeout = *abs_timeout > now ? *abs_timeout - now : eclock::duration(0);
        tasks = ext_tasks_queue->try_pop_group(pop_timeout);
    } else {
        tasks = ext_tasks_queue->pop_group();
    }

    return tasks;
}


class queue_pushing_mt_executor
{
public:
    explicit queue_pushing_mt_executor(
        shared_ptr<synchronized_queue<function<void()>>> out_tasks_queue);
    ~queue_pushing_mt_executor();

    void operator()(function<void()> task);

private:
    shared_ptr<synchronized_queue<function<void()>>> out_tasks_queue;
};


class synchronized_queue_based_event_loop : public controllable_mt_event_loop
{
public:
    synchronized_queue_based_event_loop();

    unique_ptr<activity_handle> invoke_at(eclock::time_point time, function<void()> &&task) override;
    eclock::time_point get_etime() const override;

    error_code run_iterations() override;
    void quit() override;

    function<void(function<void()>)> make_mt_executor() override;

private:
    shared_ptr<synchronized_queue<function<void()>>> ext_tasks_queue;
    weak_ptr<queue_pushing_mt_executor> mt_executor_wptr;
    eclock clock;
    unique_ptr<manual_event_loop> sub_evloop;
    eclock::time_point etime;
    bool quit_request_pending;
};


queue_pushing_mt_executor::queue_pushing_mt_executor(
    shared_ptr<synchronized_queue<function<void()>>> out_tasks_queue)
    : out_tasks_queue(move(out_tasks_queue))
{
}


queue_pushing_mt_executor::~queue_pushing_mt_executor()
{
    out_tasks_queue->push(nullptr);
}


void queue_pushing_mt_executor::operator()(function<void()> task)
{
    if (!task) {
        throw_logic_error("Executor got null task");
    }
    out_tasks_queue->push(move(task));
}


synchronized_queue_based_event_loop::synchronized_queue_based_event_loop()
    : ext_tasks_queue(), mt_executor_wptr(), clock(),
    sub_evloop(make_unique<manual_event_loop>(clock.now())),
    etime(sub_evloop->get_etime()), quit_request_pending(false)
{
}

unique_ptr<activity_handle> synchronized_queue_based_event_loop::invoke_at(eclock::time_point time, function<void()> &&task)
{
    return sub_evloop->invoke_at(time, move(task));
}


eclock::time_point synchronized_queue_based_event_loop::get_etime() const
{
    return etime;
}


error_code synchronized_queue_based_event_loop::run_iterations()
{
    error_code ret_code;

    while (true) {
        sub_evloop->process_time_passage(
            std::max<eclock::duration>(etime - sub_evloop->get_etime(), eclock::duration(0)));
        if (quit_request_pending) {
            quit_request_pending = false;
            ret_code = make_error_code(errc::interrupted);
            break;
        }

        auto next_task_time = sub_evloop->get_earliest_task_time();
        if (ext_tasks_queue) {
            auto ext_tasks = acquire_ext_tasks_with_abs_timeout(ext_tasks_queue, clock, next_task_time);
            for (auto task_iter = ext_tasks.begin(); task_iter != ext_tasks.end(); ++task_iter) {
                if (*task_iter) {
                    invoke_at(etime, move(*task_iter));
                } else {
                    ext_tasks_queue.reset();
                    mt_executor_wptr.reset();
                }
            }
        } else if (next_task_time) {
            auto now = clock.now();
            sleep_for(*next_task_time > now ? *next_task_time - now : eclock::duration(0));
        } else {
            break;
        }

        etime = clock.now();
    }

    return ret_code;
}


void synchronized_queue_based_event_loop::quit()
{
    sub_evloop->quit();
    quit_request_pending = true;
}


function<void(function<void()>)> synchronized_queue_based_event_loop::make_mt_executor()
{
    if (!ext_tasks_queue) {
        ext_tasks_queue = make_shared<synchronized_queue<function<void()>>>();
    }

    auto mt_executor = mt_executor_wptr.lock();
    if (!mt_executor) {
        mt_executor = make_shared<queue_pushing_mt_executor>(ext_tasks_queue);
        mt_executor_wptr = mt_executor;
    }

    return [mt_executor = move(mt_executor)](function<void()> task) {
        (*mt_executor)(move(task));
    };
}

}


shared_ptr<controllable_mt_event_loop> make_synchronized_queue_based_event_loop()
{
    return make_shared<synchronized_queue_based_event_loop>();
}

}
