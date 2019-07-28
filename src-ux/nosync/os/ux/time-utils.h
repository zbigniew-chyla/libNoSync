// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__TIME_UTILS_H
#define NOSYNC__OS__UX__TIME_UTILS_H

#include <chrono>
#include <ctime>


namespace nosync::os::ux
{

template<typename Rep, typename Period>
constexpr ::timespec make_timespec_from_duration(const std::chrono::duration<Rep, Period> &duration) noexcept;

std::chrono::nanoseconds make_duration_from_timespec(const ::timespec &ts) noexcept;

template<typename Rep, typename Period>
constexpr ::timeval make_timeval_from_duration(const std::chrono::duration<Rep, Period> &duration) noexcept;

std::chrono::microseconds make_duration_from_timeval(const ::timeval &tv) noexcept;

}

#include <nosync/os/ux/time-utils-impl.h>

#endif
