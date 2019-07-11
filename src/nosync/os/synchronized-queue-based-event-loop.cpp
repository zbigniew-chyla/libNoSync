#include <algorithm>
#include <iterator>
#include <nosync/manual-event-loop.h>
#include <nosync/os/synchronized-queue-based-event-loop.h>
#include <optional>
#include <system_error>
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
using std::unique_ptr;


namespace nosync::os
{

namespace
{

deque<function<void()>> acquire_ext_tasks_with_abs_timeout(
    const shared_ptr<synchronized_queue<function<void()>>> &ext_tasks_queue,
    const eclock &clock, optional<eclock::time_point> abs_timeout)
{
    deque<function<void()>> tasks;
    while (true) {
        auto new_tasks = abs_timeout.has_value()
            ? ext_tasks_queue->try_pop_group(
                std::max<eclock::duration>(*abs_timeout - clock.now(), eclock::duration(0)))
            : ext_tasks_queue->pop_group();
        if (new_tasks.empty()) {
            break;
        }

        tasks.insert(tasks.end(), move_iterator(new_tasks.begin()), move_iterator(new_tasks.end()));
    }

    return tasks;
}


class synchronized_queue_based_event_loop : public controllable_event_loop
{
public:
    synchronized_queue_based_event_loop(shared_ptr<synchronized_queue<function<void()>>> ext_tasks_queue);

    unique_ptr<activity_handle> invoke_at(eclock::time_point time, function<void()> &&task) override;
    eclock::time_point get_etime() const override;

    error_code run_iterations() override;
    void quit() override;

private:
    shared_ptr<synchronized_queue<function<void()>>> ext_tasks_queue;
    eclock clock;
    unique_ptr<manual_event_loop> sub_evloop;
    eclock::time_point etime;
    bool quit_request_pending;
};


synchronized_queue_based_event_loop::synchronized_queue_based_event_loop(
    shared_ptr<synchronized_queue<function<void()>>> ext_tasks_queue)
    : ext_tasks_queue(move(ext_tasks_queue)), clock(),
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

        auto ext_tasks = acquire_ext_tasks_with_abs_timeout(
            ext_tasks_queue, clock, sub_evloop->get_earliest_task_time());
        for (auto &task : ext_tasks) {
            invoke_at(etime, move(task));
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

}


shared_ptr<controllable_event_loop> make_synchronized_queue_based_event_loop(
    shared_ptr<synchronized_queue<function<void()>>> ext_tasks_queue)
{
    return make_shared<synchronized_queue_based_event_loop>(move(ext_tasks_queue));
}

}
