// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__CONTROLLABLE_MT_EVENT_LOOP_H
#define NOSYNC__OS__CONTROLLABLE_MT_EVENT_LOOP_H

#include <nosync/controllable-event-loop.h>
#include <nosync/os/event-loop-mt-executor-factory.h>


namespace nosync::os
{

class controllable_mt_event_loop : public controllable_event_loop, public event_loop_mt_executor_factory
{
};

}

#endif
