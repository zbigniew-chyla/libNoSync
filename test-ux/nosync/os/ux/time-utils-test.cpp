// This file is part of libnosync library. See LICENSE file for license details.
#include <gtest/gtest.h>
#include <nosync/os/ux/time-utils.h>

namespace ch = std::chrono;
using namespace std::chrono_literals;
using nosync::os::ux::make_duration_from_timespec;
using nosync::os::ux::make_duration_from_timeval;
using nosync::os::ux::make_timespec_from_duration;
using nosync::os::ux::make_timeval_from_duration;


TEST(NosyncTimeUtils, MakeTimespecFromDuration)
{
    timespec ts1 = make_timespec_from_duration(987s + 150'000'000ns);
    ASSERT_EQ(ts1.tv_sec, 987);
    ASSERT_EQ(ts1.tv_nsec, 150'000'000);

    timespec ts2 = make_timespec_from_duration(-987s - 150'000'000ns);
    ASSERT_EQ(ts2.tv_sec, -987);
    ASSERT_EQ(ts2.tv_nsec, -150'000'000);
}


TEST(NosyncTimeUtils, MakeTimeDurationFromTimespec)
{
    ASSERT_EQ(make_duration_from_timespec(timespec{987, 999'000'000}), 987s + 999'000'000ns);
    ASSERT_EQ(make_duration_from_timespec(timespec{-987, -999'000'000}), -987s - 999'000'000ns);
}


TEST(NosyncTimeUtils, MakeTimeDurationFromTimespecFromDuration)
{
    constexpr auto duration1 = 987s + 999'000'000ns;
    ASSERT_EQ(make_duration_from_timespec(make_timespec_from_duration(duration1)), duration1);

    constexpr auto duration2 = -987s - 999'000'000ns;
    ASSERT_EQ(make_duration_from_timespec(make_timespec_from_duration(duration2)), duration2);
}


TEST(NosyncTimeUtils, MakeTimevalFromDuration)
{
    timeval tv1 = make_timeval_from_duration(987s + 350'000us);
    ASSERT_EQ(tv1.tv_sec, 987);
    ASSERT_EQ(tv1.tv_usec, 350'000);

    timeval tv2 = make_timeval_from_duration(-987s - 350'000us);
    ASSERT_EQ(tv2.tv_sec, -987);
    ASSERT_EQ(tv2.tv_usec, -350'000);
}


TEST(NosyncTimeUtils, MakeTimeDurationFromTimeval)
{
    ASSERT_EQ(make_duration_from_timeval(timeval{987, 999'000}), 987s + 999'000us);
    ASSERT_EQ(make_duration_from_timeval(timeval{-987, -999'000}), -987s - 999'000us);
}


TEST(NosyncTimeUtils, MakeTimeDurationFromTimevalFromDuration)
{
    constexpr auto duration1 = 987s + 999'000us;
    ASSERT_EQ(make_duration_from_timeval(make_timeval_from_duration(duration1)), duration1);
    constexpr auto duration2 = -987s - 999'000us;
    ASSERT_EQ(make_duration_from_timeval(make_timeval_from_duration(duration2)), duration2);
}
