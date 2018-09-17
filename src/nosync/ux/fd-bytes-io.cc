#include <nosync/buffered-bytes-reader.h>
#include <nosync/reader-writer-bytes-io.h>
#include <nosync/type-utils.h>
#include <nosync/ux/fd-bytes-io.h>
#include <nosync/ux/fd-bytes-reader.h>
#include <nosync/ux/fd-bytes-writer.h>

using std::shared_ptr;
using std::size_t;


namespace nosync::ux
{

shared_ptr<bytes_io> make_fd_bytes_io(fd_watching_event_loop &evloop, shared_fd &&fd, size_t read_buffer_size)
{
    return make_reader_writer_bytes_io(
        make_buffered_bytes_reader(read_buffer_size, make_fd_bytes_reader(evloop, make_copy(fd))),
        make_fd_bytes_writer(evloop, make_copy(fd)));
}

}
