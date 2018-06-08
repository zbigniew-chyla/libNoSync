// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__EVENT_LOOP_MT_EXECUTOR_H
#define NOSYNC__EVENT_LOOP_MT_EXECUTOR_H

#include <functional>
#include <nosync/fd-watching-event-loop.h>
#include <nosync/result.h>


namespace nosync
{

result<std::function<void(std::function<void()>)>> make_event_loop_mt_executor(fd_watching_event_loop &evloop);

}

#endif /* NOSYNC__EVENT_LOOP_MT_EXECUTOR_H */
