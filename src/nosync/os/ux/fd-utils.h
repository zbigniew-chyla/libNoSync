// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__FD_UTILS_H
#define NOSYNC__OS__UX__FD_UTILS_H

#include <cstddef>
#include <nosync/result.h>
#include <string>
#include <string_view>


namespace nosync::os::ux
{

/*!
Wrapper around POSIX read() that returns the number of read bytes as result<>.

The function automatically handles EINTR from system call (by retrying it).
*/
result<std::size_t> read_some_bytes_from_fd(int fd, char *buffer, std::size_t buffer_size);


/*!
Wrapper around POSIX read() that returns data as result<>.

The function automatically handles EINTR from system call (by retrying it).
*/
result<std::string> read_some_bytes_from_fd(int fd, std::size_t max_size);


/*!
Wrapper around POSIX write() that returns status as result<>.

The function automatically handles EINTR from system call (by retrying it).
*/
result<std::size_t> write_some_bytes_to_fd(int fd, std::string_view data);

}

#endif
