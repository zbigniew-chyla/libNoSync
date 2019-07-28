// This file is part of libnosync library. See LICENSE file for license details.
#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include <nosync/time-utils.h>

namespace ch = std::chrono;
using namespace std::chrono_literals;
using namespace std::string_literals;
using nosync::format_time_to_gmtime_microseconds;
using nosync::format_time_to_gmtime_microseconds_array;
using nosync::format_time_to_gmtime_seconds;
using nosync::format_time_to_gmtime_seconds_array;
using nosync::time_point_sat_add;
using nosync::to_float_seconds;


namespace
{

template<typename T, std::size_t N>
std::array<T, N> to_array(const T (&arr)[N])
{
    std::array<T, N> result;
    std::copy(std::begin(arr), std::end(arr), result.begin());
    return result;
}

}


TEST(NosyncTimeUtils, ToFloatSeconds)
{
    EXPECT_FLOAT_EQ(to_float_seconds(ch::duration<double>(5.0)), 5.0);

    EXPECT_FLOAT_EQ(to_float_seconds(1s), 1.0);
    EXPECT_FLOAT_EQ(to_float_seconds(-3s), -3.0);

    EXPECT_FLOAT_EQ(to_float_seconds(250ms), 0.25);
    EXPECT_FLOAT_EQ(to_float_seconds(-500ms), -0.5);
}


TEST(NosyncTimeUtils, TestTimePointSatAddBasic)
{
    using time_point = ch::time_point<ch::steady_clock>;

    ASSERT_EQ(time_point_sat_add(time_point::max(), 0ns), time_point::max());
    ASSERT_EQ(time_point_sat_add(time_point::max() - 20s, 10s), time_point::max() - 10s);
    ASSERT_EQ(time_point_sat_add(time_point(1000s), 10s), time_point(1010s));

    ASSERT_EQ(time_point_sat_add(time_point::min(), 0ns), time_point::min());
    ASSERT_EQ(time_point_sat_add(time_point::min() + 20s, -10s), time_point::min() + 10s);
    ASSERT_EQ(time_point_sat_add(time_point(-1000s), -10s), time_point(-1010s));

    ASSERT_LT(time_point_sat_add(time_point::max(), -1ns), time_point::max());
    ASSERT_GT(time_point_sat_add(time_point::min(), 1ns), time_point::min());
}


TEST(NosyncTimeUtils, TestTimePointSatAddWithOverflow)
{
    using time_point = ch::time_point<ch::steady_clock>;

    ASSERT_EQ(time_point_sat_add(time_point::max(), 1ns), time_point::max());
    ASSERT_EQ(time_point_sat_add(time_point::max() - 20s, 30s), time_point::max());
    ASSERT_EQ(time_point_sat_add(time_point(1000s), ch::nanoseconds::max()), time_point::max());
    ASSERT_EQ(time_point_sat_add(time_point::max(), ch::nanoseconds::max()), time_point::max());
}


TEST(NosyncTimeUtils, TestTimePointSatAddWithUnderflow)
{
    using time_point = ch::time_point<ch::steady_clock>;

    ASSERT_EQ(time_point_sat_add(time_point::min(), -1ns), time_point::min());
    ASSERT_EQ(time_point_sat_add(time_point::min() + 20s, -30s), time_point::min());
    ASSERT_EQ(time_point_sat_add(time_point(-1000s), ch::nanoseconds::min()), time_point::min());
    ASSERT_EQ(time_point_sat_add(time_point::min(), ch::nanoseconds::min()), time_point::min());
}


TEST(NosyncTimeUtils, TestFormatTimeToGmtimeSeconds)
{
    ASSERT_EQ(format_time_to_gmtime_seconds(ch::system_clock::from_time_t(1509128597)), "2017-10-27T18:23:17"s);
    ASSERT_EQ(format_time_to_gmtime_seconds(ch::system_clock::from_time_t(1488589690)), "2017-03-04T01:08:10"s);
    ASSERT_EQ(format_time_to_gmtime_seconds(ch::system_clock::from_time_t(1488589690) + 3ms), "2017-03-04T01:08:10"s);
    ASSERT_EQ(format_time_to_gmtime_seconds(ch::system_clock::from_time_t(1488589690) + 7us), "2017-03-04T01:08:10"s);
}


TEST(NosyncTimeUtils, TestFormatTimeToGmtimeMicroseconds)
{
    ASSERT_EQ(format_time_to_gmtime_microseconds(ch::system_clock::from_time_t(1488589690)), "2017-03-04T01:08:10.000000"s);
    ASSERT_EQ(format_time_to_gmtime_microseconds(ch::system_clock::from_time_t(1488589690) + 3ms), "2017-03-04T01:08:10.003000"s);
    ASSERT_EQ(format_time_to_gmtime_microseconds(ch::system_clock::from_time_t(1488589690) + 7us), "2017-03-04T01:08:10.000007"s);
    ASSERT_EQ(format_time_to_gmtime_microseconds(ch::system_clock::from_time_t(1488589690) + 987654us), "2017-03-04T01:08:10.987654"s);
}


TEST(NosyncTimeUtils, TestFormatTimeToGmtimeSecondsArray)
{
    ASSERT_EQ(format_time_to_gmtime_seconds_array(ch::system_clock::from_time_t(1509128597)), to_array("2017-10-27T18:23:17"));
    ASSERT_EQ(format_time_to_gmtime_seconds_array(ch::system_clock::from_time_t(1488589690)), to_array("2017-03-04T01:08:10"));
    ASSERT_EQ(format_time_to_gmtime_seconds_array(ch::system_clock::from_time_t(1488589690) + 3ms), to_array("2017-03-04T01:08:10"));
    ASSERT_EQ(format_time_to_gmtime_seconds_array(ch::system_clock::from_time_t(1488589690) + 7us), to_array("2017-03-04T01:08:10"));
}


TEST(NosyncTimeUtils, TestFormatTimeToGmtimeMicrosecondsArray)
{
    ASSERT_EQ(format_time_to_gmtime_microseconds_array(ch::system_clock::from_time_t(1488589690)), to_array("2017-03-04T01:08:10.000000"));
    ASSERT_EQ(format_time_to_gmtime_microseconds_array(ch::system_clock::from_time_t(1488589690) + 3ms), to_array("2017-03-04T01:08:10.003000"));
    ASSERT_EQ(format_time_to_gmtime_microseconds_array(ch::system_clock::from_time_t(1488589690) + 7us), to_array("2017-03-04T01:08:10.000007"));
    ASSERT_EQ(format_time_to_gmtime_microseconds_array(ch::system_clock::from_time_t(1488589690) + 987654us), to_array("2017-03-04T01:08:10.987654"));
}
