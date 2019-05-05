// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__TIMEOUTING_TASKS_QUEUE_H
#define NOSYNC__TIMEOUTING_TASKS_QUEUE_H

#include <deque>
#include <functional>
#include <memory>
#include <nosync/activity-owner.h>
#include <nosync/eclock.h>
#include <nosync/event-loop.h>
#include <optional>
#include <system_error>
#include <tuple>


namespace nosync
{

class timeouting_tasks_queue
{
public:
    explicit timeouting_tasks_queue(event_loop &evloop);
    ~timeouting_tasks_queue();

    timeouting_tasks_queue(timeouting_tasks_queue &&) = default;

    timeouting_tasks_queue &operator=(timeouting_tasks_queue &&) = delete;

    timeouting_tasks_queue(const timeouting_tasks_queue &) = delete;
    timeouting_tasks_queue &operator=(const timeouting_tasks_queue &) = delete;

    void push_task(eclock::duration timeout, std::function<void(std::error_code)> &&task);
    bool has_tasks() const;
    void run_next_task();

private:
    void process_expired_tasks();
    void reschedule_expiration_task_at(eclock::time_point timeout_end);
    void reschedule_expiration_task();

    event_loop &evloop;
    std::deque<std::tuple<eclock::time_point, std::function<void(std::error_code)>>> tasks_entries;
    std::optional<std::tuple<eclock::time_point, activity_owner>> expiration_task_state;
};

}

#endif /* NOSYNC__TIMEOUTING_TASKS_QUEUE_H */
