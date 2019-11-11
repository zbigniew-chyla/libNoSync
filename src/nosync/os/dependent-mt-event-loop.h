// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__DEPENDENT_MT_EVENT_LOOP_H
#define NOSYNC__DEPENDENT_MT_EVENT_LOOP_H

#include <chrono>
#include <functional>
#include <nosync/os/mt-event-loop.h>
#include <memory>


namespace nosync::os
{

/*!
Create mt_event_loop object based on some other event loop like mechanism.
*/
std::shared_ptr<mt_event_loop> make_dependent_mt_event_loop(
    std::function<std::chrono::nanoseconds()> time_supplier,
    std::function<void(std::chrono::nanoseconds, std::function<void()>)> &&wakeup_scheduler,
    std::function<void(std::function<void()>)> &&mt_executor);

}

#endif
