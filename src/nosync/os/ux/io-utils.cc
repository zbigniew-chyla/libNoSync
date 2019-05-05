// This file is part of libnosync library. See LICENSE file for license details.
#include <experimental/array>
#include <fcntl.h>
#include <nosync/result-utils.h>
#include <nosync/os/ux/io-utils.h>
#include <system_error>
#include <unistd.h>

using std::array;
using std::experimental::make_array;


namespace nosync::os::ux
{

result<array<owned_fd, 2>> open_pipe()
{
    int pipe_fds[2];
    int pipe_retval = ::pipe2(pipe_fds, O_CLOEXEC | O_NONBLOCK);

    return pipe_retval == 0
        ? make_ok_result(make_array(owned_fd(pipe_fds[0]), owned_fd(pipe_fds[1])))
        : make_raw_error_result_from_errno();
}

}
