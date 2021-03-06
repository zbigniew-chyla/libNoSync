// This file is part of libnosync library. See LICENSE file for license details.
#include <cerrno>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <nosync/os/ux/io-utils.h>
#include <unistd.h>

using nosync::os::ux::open_pipe;
using std::array;


TEST(NosyncIoUtils, OpenPipeCheckFds)
{
    auto pipe_fds_res = open_pipe();
    ASSERT_TRUE(pipe_fds_res.is_ok());
    auto &pipe_fds = pipe_fds_res.get_value();

    ASSERT_GE(*pipe_fds[0], 0);
    ASSERT_GE(*pipe_fds[1], 0);
    auto read_open_flags = ::fcntl(*pipe_fds[0], F_GETFL);
    ASSERT_TRUE((read_open_flags & O_NONBLOCK) != 0);
    auto write_open_flags = ::fcntl(*pipe_fds[1], F_GETFL);
    ASSERT_TRUE((write_open_flags & O_NONBLOCK) != 0);
}


TEST(NosyncIoUtils, OpenPipeCheckEmptyRead)
{
    auto pipe_fds_res = open_pipe();
    ASSERT_TRUE(pipe_fds_res.is_ok());
    auto &pipe_fds = pipe_fds_res.get_value();

    errno = 0;
    array<char, 1> read_buf;
    auto read_retval = ::read(*pipe_fds[0], read_buf.data(), read_buf.size());
    ASSERT_EQ(read_retval, -1);
    ASSERT_EQ(errno, EAGAIN);
}


TEST(NosyncIoUtils, OpenPipeCheckIO)
{
    constexpr auto test_byte = '\xAD';

    auto pipe_fds_res = open_pipe();
    ASSERT_TRUE(pipe_fds_res.is_ok());
    auto &pipe_fds = pipe_fds_res.get_value();

    auto write_char = test_byte;
    auto write_retval = ::write(*pipe_fds[1], &write_char, 1);
    ASSERT_NE(write_retval, -1);

    auto read_char = static_cast<char>(~test_byte);
    auto read_retval = ::read(*pipe_fds[0], &read_char, 1);
    ASSERT_NE(read_retval, -1);
    ASSERT_EQ(read_char, test_byte);
}
