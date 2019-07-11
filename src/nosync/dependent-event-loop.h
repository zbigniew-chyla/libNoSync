// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__DEPENDENT_EVENT_LOOP_H
#define NOSYNC__DEPENDENT_EVENT_LOOP_H

#include <chrono>
#include <functional>
#include <nosync/event-loop.h>
#include <memory>


namespace nosync
{

/*!
Create event_loop object based on some other event loop like mechanism.
*/
std::shared_ptr<event_loop> make_dependent_event_loop(
    std::function<std::chrono::nanoseconds()> time_supplier,
    std::function<void(std::chrono::nanoseconds, std::function<void()>)> &&wakeup_scheduler);

}

#endif /* NOSYNC__DEPENDENT_EVENT_LOOP_H */
