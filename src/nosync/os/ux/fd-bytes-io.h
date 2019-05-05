// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__FD_BYTES_IO_H
#define NOSYNC__OS__UX__FD_BYTES_IO_H

#include <cstddef>
#include <nosync/bytes-io.h>
#include <nosync/os/ux/fd-watching-event-loop.h>
#include <nosync/os/ux/shared-fd.h>
#include <memory>


namespace nosync::os::ux
{

std::shared_ptr<bytes_io> make_fd_bytes_io(
    fd_watching_event_loop &evloop, shared_fd &&fd, std::size_t read_buffer_size = 4096);

}

#endif /* NOSYNC__OS__UX__FD_BYTES_IO_H */
