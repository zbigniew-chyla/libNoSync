// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__EVENT_LOOP_UTILS_H
#define NOSYNC__EVENT_LOOP_UTILS_H

#include <functional>
#include <nosync/eclock.h>
#include <nosync/event-loop.h>


namespace nosync
{

std::unique_ptr<activity_handle> invoke_later(event_loop &evloop, std::function<void()> &&task);
std::unique_ptr<activity_handle> invoke_with_etime_delay(
    event_loop &evloop, eclock::duration delay, std::function<void()> &&task);

}

#endif
