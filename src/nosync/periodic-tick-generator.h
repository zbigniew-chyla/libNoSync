// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__PERIODIC_TICK_GENERATOR_H
#define NOSYNC__PERIODIC_TICK_GENERATOR_H

#include <functional>
#include <nosync/eclock.h>
#include <nosync/event-loop.h>
#include <nosync/interface-type.h>
#include <memory>


namespace nosync
{

std::shared_ptr<interface_type> make_periodic_tick_generator(
    event_loop &evloop, eclock::duration interval, std::function<void()> &&tick_func);

}

#endif /* NOSYNC__PERIODIC_TICK_GENERATOR_H */
