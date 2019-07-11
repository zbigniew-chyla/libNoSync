// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__CONTROLLABLE_EVENT_LOOP_H
#define NOSYNC__CONTROLLABLE_EVENT_LOOP_H

#include <nosync/event-loop.h>
#include <nosync/event-loop-control.h>


namespace nosync
{

/*!
Interface "aggregating" event_loop and event_loop_control.
*/
class controllable_event_loop : public event_loop, public event_loop_control
{
};

}

#endif /* NOSYNC__CONTROLLABLE_EVENT_LOOP_H */
