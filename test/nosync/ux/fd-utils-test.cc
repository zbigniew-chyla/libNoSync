// This file is part of libnosync library. See LICENSE file for license details.
#include <array>
#include <cerrno>
#include <gtest/gtest.h>
#include <nosync/ux/fd-utils.h>
#include <nosync/ux/test/io-utils.h>
#include <string>
#include <system_error>
#include <unistd.h>

using namespace std::string_literals;
using nosync::ux::read_some_bytes_from_fd;
using nosync::test::read_nointr;
using nosync::test::write_nointr;
using nosync::ux::write_some_bytes_to_fd;
using std::array;
using std::error_code;
using std::generic_category;
using std::string;


namespace
{

const auto test_bytes = "abcdefghi"s;

}


TEST(NosyncFdUtils, ReadSomeBytesFromFdBadFd)
{
    auto read_res = read_some_bytes_from_fd(-1, 100);
    ASSERT_FALSE(read_res.is_ok());
    ASSERT_EQ(read_res.get_error(), error_code(EBADF, generic_category()));
}


TEST(NosyncFdUtils, ReadSomeBytesFromFdOk)
{
    array<int, 2> pipe_fds;
    ASSERT_EQ(::pipe(pipe_fds.data()), 0);
    auto write_retval = write_nointr(pipe_fds[1], test_bytes.data(), test_bytes.size());
    ASSERT_GT(write_retval, 0);
    ASSERT_EQ(static_cast<size_t>(write_retval), test_bytes.size());

    auto read_res = read_some_bytes_from_fd(pipe_fds[0], test_bytes.size() + 1);
    ASSERT_TRUE(read_res.is_ok());
    ASSERT_EQ(read_res.get_value(), test_bytes);
}


TEST(NosyncFdUtils, WriteSomeBytesToFdOk)
{
    array<int, 2> pipe_fds;
    ASSERT_EQ(::pipe(pipe_fds.data()), 0);

    auto write_res = write_some_bytes_to_fd(pipe_fds[1], test_bytes);
    ASSERT_TRUE(write_res.is_ok());
    ASSERT_EQ(write_res.get_value(), test_bytes.size());

    string read_buf(test_bytes.size(), '\xFF');
    auto read_retval = read_nointr(pipe_fds[0], &read_buf[0], read_buf.size());
    ASSERT_GT(read_retval, 0);
    ASSERT_EQ(static_cast<size_t>(read_retval), test_bytes.size());
    ASSERT_EQ(read_buf, test_bytes);
}


TEST(NosyncFdUtils, WriteSomeBytesToFdBadFd)
{
    auto write_res = write_some_bytes_to_fd(-1, "abcdef");
    ASSERT_FALSE(write_res.is_ok());
    ASSERT_EQ(write_res.get_error(), error_code(EBADF, generic_category()));
}
