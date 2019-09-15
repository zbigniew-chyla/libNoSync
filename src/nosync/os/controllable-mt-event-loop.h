// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__CONTROLLABLE_MT_EVENT_LOOP_H
#define NOSYNC__OS__CONTROLLABLE_MT_EVENT_LOOP_H

#include <nosync/controllable-event-loop.h>
#include <nosync/os/mt-event-loop.h>


namespace nosync::os
{

class controllable_mt_event_loop : public controllable_event_loop, public mt_event_loop
{
};

}

#endif
