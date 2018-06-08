// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__EVENT_LOOP_MT_EXECUTOR_H
#define NOSYNC__EVENT_LOOP_MT_EXECUTOR_H

#include <nosync/fd-watching-event-loop.h>
#include <nosync/ux/event-loop-mt-executor.h>


namespace nosync
{

using ux::make_event_loop_mt_executor;

}

#endif /* NOSYNC__EVENT_LOOP_MT_EXECUTOR_H */
