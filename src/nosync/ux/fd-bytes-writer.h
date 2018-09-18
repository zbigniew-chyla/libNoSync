// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC_UX__FD_BYTES_WRITER_H
#define NOSYNC_UX__FD_BYTES_WRITER_H

#include <nosync/bytes-writer.h>
#include <nosync/ux/fd-watcher.h>
#include <nosync/ux/shared-fd.h>
#include <memory>


namespace nosync::ux
{

std::shared_ptr<bytes_writer> make_fd_bytes_writer(fd_watcher &watcher, shared_fd &&fd);

}

#endif /* NOSYNC_UX__FD_BYTES_WRITER_H */
