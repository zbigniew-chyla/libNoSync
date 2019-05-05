// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__FD_BYTES_READER_H
#define NOSYNC__OS__UX__FD_BYTES_READER_H

#include <nosync/bytes-reader.h>
#include <nosync/os/ux/fd-watching-event-loop.h>
#include <nosync/os/ux/shared-fd.h>
#include <memory>


namespace nosync::os::ux
{

std::shared_ptr<bytes_reader> make_fd_bytes_reader(fd_watching_event_loop &evloop, shared_fd &&fd);

}

#endif /* NOSYNC__OS__UX__FD_BYTES_READER_H */
