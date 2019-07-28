// This file is part of libnosync library. See LICENSE file for license details.
#include <nosync/os/ux/time-utils.h>

namespace ch = std::chrono;


namespace nosync::os::ux
{

ch::nanoseconds make_duration_from_timespec(const ::timespec &ts) noexcept
{
    return ch::seconds(ts.tv_sec) + ch::nanoseconds(ts.tv_nsec);
}


ch::microseconds make_duration_from_timeval(const ::timeval &tv) noexcept
{
    return ch::seconds(tv.tv_sec) + ch::microseconds(tv.tv_usec);
}

}
