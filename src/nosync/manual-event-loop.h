// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__MANUAL_EVENT_LOOP_H
#define NOSYNC__MANUAL_EVENT_LOOP_H

#include <cstdint>
#include <functional>
#include <nosync/compat/optional.h>
#include <nosync/eclock.h>
#include <nosync/event-loop.h>
#include <map>
#include <memory>
#include <tuple>


namespace nosync
{

class manual_event_loop_task_handle;


class manual_event_loop : public event_loop
{
public:
    manual_event_loop(eclock::time_point init_time);
    ~manual_event_loop() override;

    compat::optional<eclock::time_point> get_earliest_task_time() const;
    bool process_time_passage(eclock::duration time_delta);
    void quit();

    bool is_enabled(const manual_event_loop_task_handle &task_handle) const;
    void disable(const manual_event_loop_task_handle &task_handle);

    std::unique_ptr<activity_handle> invoke_at(eclock::time_point time, std::function<void()> &&task) override;
    eclock::time_point get_etime() const override;

private:
    eclock::time_point last_event_time;
    std::map<std::tuple<eclock::time_point, std::uint64_t>, std::function<void()>> pending_tasks;
    std::uint64_t next_task_id;
    bool quit_request_pending;
};

}

#endif /* NOSYNC__MANUAL_EVENT_LOOP_H */
