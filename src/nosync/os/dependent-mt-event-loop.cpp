// This file is part of libnosync library. See LICENSE file for license details.
#include <nosync/dependent-event-loop.h>
#include <nosync/event-loop-utils.h>
#include <nosync/memory-utils.h>
#include <nosync/os/dependent-mt-event-loop.h>
#include <utility>

namespace ch = std::chrono;
using std::enable_shared_from_this;
using std::function;
using std::make_shared;
using std::move;
using std::shared_ptr;
using std::unique_ptr;


namespace nosync::os
{

namespace
{

class dependent_mt_event_loop : public mt_event_loop, public enable_shared_from_this<dependent_mt_event_loop>
{
public:
    dependent_mt_event_loop(
        function<ch::nanoseconds()> time_supplier,
        function<void(ch::nanoseconds time, function<void()>)> &&wakeup_scheduler,
        std::function<void(std::function<void()>)> &&mt_executor);

    unique_ptr<activity_handle> invoke_at(eclock::time_point time, function<void()> &&task) override;
    eclock::time_point get_etime() const override;
    std::function<void(std::function<void()>)> get_mt_executor() override;

private:
    shared_ptr<event_loop> sub_evloop;
    std::function<void(std::function<void()>)> mt_executor;
    std::function<void(std::function<void()>)> evloop_mt_executor;
};


dependent_mt_event_loop::dependent_mt_event_loop(
    function<ch::nanoseconds()> time_supplier,
    function<void(ch::nanoseconds time, function<void()>)> &&wakeup_scheduler,
    std::function<void(std::function<void()>)> &&mt_executor)
    : sub_evloop(make_dependent_event_loop(move(time_supplier), move(wakeup_scheduler))),
    mt_executor(move(mt_executor)), evloop_mt_executor()
{
}


unique_ptr<activity_handle> dependent_mt_event_loop::invoke_at(eclock::time_point time, function<void()> &&task)
{
    return sub_evloop->invoke_at(time, move(task));
}


eclock::time_point dependent_mt_event_loop::get_etime() const
{
    return sub_evloop->get_etime();
}


std::function<void(std::function<void()>)> dependent_mt_event_loop::get_mt_executor()
{
    if (!evloop_mt_executor) {
        evloop_mt_executor = [self_wptr = weak_from_this(), base_mt_executor = move(mt_executor)](std::function<void()> task) {
            base_mt_executor(
                [self_wptr, shared_task = move_to_shared(move(task))]() mutable {
                    auto self_ptr = self_wptr.lock();
                    if (self_ptr) {
                        invoke_later(
                            *self_ptr->sub_evloop,
                            [shared_task = move(shared_task)]() {
                                (*shared_task)();
                            });
                    }
                });
        };
    }

    return evloop_mt_executor;
}

}


shared_ptr<mt_event_loop> make_dependent_mt_event_loop(
    function<ch::nanoseconds()> time_supplier,
    function<void(ch::nanoseconds, function<void()>)> &&wakeup_scheduler,
    std::function<void(std::function<void()>)> &&mt_executor)
{
    return make_shared<dependent_mt_event_loop>(
        move(time_supplier), move(wakeup_scheduler), move(mt_executor));
}

}
