// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__TIME_UTILS_IMPL_H
#define NOSYNC__TIME_UTILS_IMPL_H


namespace nosync
{

template<typename Rep, typename Period>
constexpr double to_float_seconds(const std::chrono::duration<Rep, Period> &duration) noexcept
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(duration).count();
}


template<class Clock, class Duration>
std::chrono::time_point<Clock> time_point_sat_add(std::chrono::time_point<Clock> time, Duration delta)
{
    using time_point = std::chrono::time_point<Clock>;
    constexpr auto zero_time = time_point(Duration::zero());

    time_point sat_sum;
    if (time >= zero_time && delta > (time_point::max() - time)) {
        sat_sum = time_point::max();
    } else if (time < zero_time && delta < (time_point::min() - time)) {
        sat_sum = time_point::min();
    } else {
        sat_sum = time + delta;
    }

    return sat_sum;
}

}

#endif
