// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__SYNCHRONIZED_QUEUE_BASED_EVENT_LOOP_H
#define NOSYNC__OS__SYNCHRONIZED_QUEUE_BASED_EVENT_LOOP_H

#include <functional>
#include <memory>
#include <nosync/os/controllable-mt-event-loop.h>


namespace nosync::os
{

/*!
Create event_loop object that can accept tasks from other threads via a queue.
*/
std::shared_ptr<controllable_mt_event_loop> make_synchronized_queue_based_event_loop();

}

#endif
