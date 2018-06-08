// This file is part of libnosync library. See LICENSE file for license details.
#include <experimental/array>
#include <fcntl.h>
#include <nosync/exceptions.h>
#include <nosync/io-utils.h>
#include <nosync/result-utils.h>
#include <system_error>
#include <unistd.h>

using std::array;
using std::experimental::make_array;


namespace nosync
{

array<owned_fd, 2> create_nonblocking_pipe()
{
    return get_result_value_or_throw(open_pipe(), "failed to create pipe");
}


result<array<owned_fd, 2>> open_pipe()
{
    int pipe_fds[2];
    int pipe_retval = ::pipe2(pipe_fds, O_CLOEXEC | O_NONBLOCK);

    return pipe_retval == 0
        ? make_ok_result(make_array(owned_fd(pipe_fds[0]), owned_fd(pipe_fds[1])))
        : make_raw_error_result_from_errno();
}

}
