// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__MT_EVENT_LOOP_H
#define NOSYNC__OS__MT_EVENT_LOOP_H

#include <nosync/event-loop.h>
#include <nosync/os/event-loop-mt-executor-provider.h>


namespace nosync::os
{

class mt_event_loop : public virtual event_loop, public event_loop_mt_executor_provider
{
};

}

#endif
