// This file is part of libnosync library. See LICENSE file for license details.
#include <cerrno>
#include <nosync/os/ux/test/io-utils.h>
#include <unistd.h>


namespace nosync
{
namespace test
{

long read_nointr(int fd, void *buf, size_t count)
{
    ssize_t read_retval;
    do {
        read_retval = ::read(fd, buf, count);
    } while (read_retval == -1 && errno == EINTR);

    return read_retval;
}


long write_nointr(int fd, const void *buf, size_t count)
{
    ssize_t write_retval;
    do {
        write_retval = ::write(fd, buf, count);
    } while (write_retval == -1 && errno == EINTR);

    return write_retval;
}

}
}
