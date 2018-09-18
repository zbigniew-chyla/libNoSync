// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__PPOLL_BASED_EVENT_LOOP_H
#define NOSYNC__PPOLL_BASED_EVENT_LOOP_H

#include <nosync/full-fd-watching-event-loop.h>
#include <nosync/ux/ppoll-based-event-loop.h>


namespace nosync
{

using ux::make_ppoll_based_event_loop;

}

#endif /* NOSYNC__PPOLL_BASED_EVENT_LOOP_H */
