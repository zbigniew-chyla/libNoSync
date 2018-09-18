// This file is part of libnosync library. See LICENSE file for license details.
#include <array>
#include <cerrno>
#include <memory>
#include <nosync/raw-error-result.h>
#include <nosync/result-utils.h>
#include <nosync/ux/fd-utils.h>
#include <unistd.h>

using std::array;
using std::make_unique;
using std::size_t;
using std::string;
using std::string_view;


namespace nosync::ux
{

namespace
{

constexpr auto max_local_buffer_size = 8192U;


result<string> read_some_bytes_using_tmp_buffer(int fd, char *tmp_buffer, size_t tmp_buffer_size)
{
    auto read_size_res = read_some_bytes_from_fd(fd, tmp_buffer, tmp_buffer_size);
    return read_size_res.is_ok()
        ? make_ok_result(string(tmp_buffer, read_size_res.get_value()))
        : raw_error_result(read_size_res);
}

}


result<size_t> read_some_bytes_from_fd(int fd, char *buffer, size_t buffer_size)
{
    ssize_t read_res;
    do {
        read_res = ::read(fd, buffer, buffer_size);
    } while (read_res == -1 && errno == EINTR);

    return read_res >= 0
        ? make_ok_result(static_cast<size_t>(read_res))
        : make_raw_error_result_from_errno();
}


result<string> read_some_bytes_from_fd(int fd, size_t max_size)
{
    return read_some_bytes_using_tmp_buffer(
        fd,
        max_size <= max_local_buffer_size
            ? array<char, max_local_buffer_size>().data()
            : make_unique<char[]>(max_size).get(),
        max_size);
}


result<size_t> write_some_bytes_to_fd(int fd, string_view data)
{
    ssize_t write_res;
    do {
        write_res = ::write(fd, data.data(), data.size());
    } while (write_res == -1 && errno == EINTR);

    return write_res >= 0
        ? make_ok_result(static_cast<size_t>(write_res))
        : make_raw_error_result_from_errno();
}

}
