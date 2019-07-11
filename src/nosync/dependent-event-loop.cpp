// This file is part of libnosync library. See LICENSE file for license details.
#include <nosync/dependent-event-loop.h>
#include <nosync/manual-event-loop.h>
#include <nosync/memory-utils.h>
#include <optional>
#include <utility>

namespace ch = std::chrono;
using std::enable_shared_from_this;
using std::function;
using std::make_shared;
using std::make_unique;
using std::move;
using std::optional;
using std::shared_ptr;
using std::unique_ptr;


namespace nosync
{

namespace
{

class dependent_event_loop : public event_loop, public enable_shared_from_this<dependent_event_loop>
{
public:
    dependent_event_loop(
        function<ch::nanoseconds()> time_supplier,
        function<void(ch::nanoseconds time, function<void()>)> &&wakeup_scheduler);

    unique_ptr<activity_handle> invoke_at(eclock::time_point time, function<void()> &&task) override;
    eclock::time_point get_etime() const override;

private:
    void schedule_wakeup_if_needed(eclock::time_point wakeup_time);
    void handle_wakeup();

    function<ch::nanoseconds()> time_supplier;
    function<void(ch::nanoseconds time, function<void()>)> wakeup_scheduler;
    unique_ptr<manual_event_loop> sub_evloop;
    optional<eclock::time_point> scheduled_wakeup_time;
};


dependent_event_loop::dependent_event_loop(
    function<ch::nanoseconds()> time_supplier,
    function<void(ch::nanoseconds time, function<void()>)> &&wakeup_scheduler)
    : time_supplier(move(time_supplier)), wakeup_scheduler(move(wakeup_scheduler)),
    sub_evloop(make_unique<manual_event_loop>(eclock::time_point(this->time_supplier()))),
    scheduled_wakeup_time()
{
}


unique_ptr<activity_handle> dependent_event_loop::invoke_at(eclock::time_point time, function<void()> &&task)
{
    schedule_wakeup_if_needed(time);

    return sub_evloop->invoke_at(time, move(task));
}


eclock::time_point dependent_event_loop::get_etime() const
{
    return sub_evloop->get_etime();
}


void dependent_event_loop::schedule_wakeup_if_needed(eclock::time_point wakeup_time)
{
    if (scheduled_wakeup_time && scheduled_wakeup_time <= wakeup_time) {
        return;
    }

    wakeup_scheduler(
        ch::duration_cast<ch::nanoseconds>(wakeup_time.time_since_epoch()),
        make_weak_this_func_proxy(
            this,
            [](auto &this_ref) {
                this_ref.scheduled_wakeup_time.reset();
                this_ref.handle_wakeup();
            }));
    scheduled_wakeup_time = wakeup_time;
}


void dependent_event_loop::handle_wakeup()
{
    auto earliest_task_time = sub_evloop->get_earliest_task_time();
    if (!earliest_task_time) {
        return;
    }

    auto external_time = eclock::time_point(ch::duration_cast<eclock::duration>(time_supplier()));
    if (external_time >= *earliest_task_time) {
        sub_evloop->process_time_passage(external_time - sub_evloop->get_etime());
        auto next_wakeup_time = sub_evloop->get_earliest_task_time();
        if (next_wakeup_time) {
            schedule_wakeup_if_needed(*next_wakeup_time);
        }
    } else {
        schedule_wakeup_if_needed(*earliest_task_time);
    }
}

}


shared_ptr<event_loop> make_dependent_event_loop(
    function<ch::nanoseconds()> time_supplier,
    function<void(ch::nanoseconds, function<void()>)> &&wakeup_scheduler)
{
    return make_shared<dependent_event_loop>(move(time_supplier), move(wakeup_scheduler));
}

}
