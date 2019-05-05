// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__UX__FD_BYTES_READER_H
#define NOSYNC__UX__FD_BYTES_READER_H

#include <nosync/bytes-reader.h>
#include <nosync/ux/fd-watching-event-loop.h>
#include <nosync/ux/shared-fd.h>
#include <memory>


namespace nosync::ux
{

std::shared_ptr<bytes_reader> make_fd_bytes_reader(fd_watching_event_loop &evloop, shared_fd &&fd);

}

#endif /* NOSYNC__UX__FD_BYTES_READER_H */
