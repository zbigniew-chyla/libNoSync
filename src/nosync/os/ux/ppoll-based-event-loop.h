// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__PPOLL_BASED_EVENT_LOOP_H
#define NOSYNC__OS__UX__PPOLL_BASED_EVENT_LOOP_H

#include <nosync/os/ux/full-fd-watching-event-loop.h>
#include <memory>


namespace nosync::os::ux
{

std::shared_ptr<full_fd_watching_event_loop> make_ppoll_based_event_loop();

}

#endif /* NOSYNC__OS__UX__PPOLL_BASED_EVENT_LOOP_H */
