// This file is part of libnosync library. See LICENSE file for license details.
#include <algorithm>
#include <nosync/time-limited-bytes-reader.h>
#include <utility>

using std::make_shared;
using std::shared_ptr;
using std::string;


namespace nosync
{

namespace
{

class time_limited_bytes_reader : public bytes_reader
{
public:
    time_limited_bytes_reader(
        event_loop &evloop, eclock::time_point timeout_end,
        shared_ptr<bytes_reader> &&base_reader);

    void read_some_bytes(
        size_t max_size, eclock::duration timeout,
        result_handler<string> &&res_handler) override;

private:
    event_loop &evloop;
    eclock::time_point timeout_end;
    shared_ptr<bytes_reader> base_reader;
};


time_limited_bytes_reader::time_limited_bytes_reader(
    event_loop &evloop, eclock::time_point timeout_end,
    shared_ptr<bytes_reader> &&base_reader)
    : evloop(evloop), timeout_end(timeout_end), base_reader(move(base_reader))
{
}


void time_limited_bytes_reader::read_some_bytes(
    size_t max_size, eclock::duration timeout,
    result_handler<string> &&res_handler)
{
    const auto time_left = std::max(timeout_end - evloop.get_etime(), eclock::duration(0));
    base_reader->read_some_bytes(max_size, std::min(timeout, time_left), move(res_handler));
}

}


shared_ptr<bytes_reader> make_time_limited_bytes_reader(
    event_loop &evloop, eclock::time_point timeout_end,
    shared_ptr<bytes_reader> &&base_reader)
{
    return make_shared<time_limited_bytes_reader>(evloop, timeout_end, move(base_reader));
}

}
