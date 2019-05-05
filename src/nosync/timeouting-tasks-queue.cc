#include <algorithm>
#include <iterator>
#include <nosync/event-loop-utils.h>
#include <nosync/time-utils.h>
#include <nosync/timeouting-tasks-queue.h>

using std::errc;
using std::error_code;
using std::function;
using std::get;
using std::make_error_code;
using std::move;
using std::nullopt;
using std::vector;


namespace nosync
{

timeouting_tasks_queue::timeouting_tasks_queue(event_loop &evloop)
    : evloop(evloop), tasks_entries(), expiration_task_state()
{
}


timeouting_tasks_queue::~timeouting_tasks_queue()
{
    if (tasks_entries.empty()) {
        return;
    }

    invoke_later(
        evloop,
        [tasks_entries = move(tasks_entries)]() mutable {
            for (auto &task_entry : tasks_entries) {
                auto &task = get<function<void(error_code)>>(task_entry);
                task(make_error_code(errc::operation_canceled));
                task = nullptr;
            }
        });
}


void timeouting_tasks_queue::push_task(eclock::duration timeout, function<void(error_code)> &&task)
{
    auto timeout_end = time_point_sat_add(evloop.get_etime(), timeout);

    if (!expiration_task_state || get<eclock::time_point>(*expiration_task_state) > timeout_end) {
        reschedule_expiration_task_at(timeout_end);
    }

    tasks_entries.emplace_back(timeout_end, move(task));
}


bool timeouting_tasks_queue::has_tasks() const
{
    return !tasks_entries.empty();
}


void timeouting_tasks_queue::run_next_task()
{
    if (tasks_entries.empty()) {
        return;
    }

    auto task_entry = move(tasks_entries.front());
    tasks_entries.pop_front();

    reschedule_expiration_task();

    auto &task = get<function<void(error_code)>>(task_entry);
    task(error_code());
}


void timeouting_tasks_queue::process_expired_tasks()
{
    auto expired_tasks_entries_begin_iter = std::stable_partition(
        tasks_entries.begin(), tasks_entries.end(),
        [now = evloop.get_etime()](const auto &task) {
            return get<eclock::time_point>(task) > now;
        });

    auto expired_tasks_entries = vector(
        std::make_move_iterator(expired_tasks_entries_begin_iter),
        std::make_move_iterator(tasks_entries.end()));
    tasks_entries.erase(expired_tasks_entries_begin_iter, tasks_entries.end());

    for (auto &task_entry : expired_tasks_entries) {
        auto &task = get<function<void(error_code)>>(task_entry);
        task(make_error_code(errc::timed_out));
        task = nullptr;
    }
}


void timeouting_tasks_queue::reschedule_expiration_task_at(eclock::time_point timeout_end)
{
    expiration_task_state = nullopt;
    expiration_task_state.emplace(
        timeout_end,
        evloop.invoke_at(
            timeout_end,
            [this]() {
                expiration_task_state = nullopt;
                process_expired_tasks();
                reschedule_expiration_task();
            }));
}


void timeouting_tasks_queue::reschedule_expiration_task()
{
    if (tasks_entries.empty()) {
        expiration_task_state = nullopt;
        return;
    }

    auto min_timeout_task_entry_iter = std::min_element(
        tasks_entries.cbegin(), tasks_entries.cend(),
        [](const auto &lhs, const auto &rhs) {
            return get<eclock::time_point>(lhs) < get<eclock::time_point>(rhs);
        });
    auto min_timeout_end = get<eclock::time_point>(*min_timeout_task_entry_iter);

    if (!expiration_task_state || get<eclock::time_point>(*expiration_task_state) != min_timeout_end) {
        reschedule_expiration_task_at(min_timeout_end);
    }
}

}
