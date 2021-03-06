// This file is part of libnosync library. See LICENSE file for license details.
#include <gtest/gtest.h>
#include <nosync/result.h>
#include <nosync/test/macros.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>

using namespace std::string_literals;
using nosync::make_error_result;
using nosync::make_ok_result;
using std::add_const_t ;
using std::errc;
using std::is_same_v;
using std::make_error_code;
using std::move;
using std::string;


TEST(NosyncResult, UseWithError)
{
    auto ec = make_error_code(errc::interrupted);
    auto res = make_error_result<string>(ec);
    ASSERT_FALSE(res.is_ok());
    ASSERT_EQ(res.get_error(), ec);
#if NOSYNC_TEST_EXCEPTIONS_ENABLED
    ASSERT_THROW(res.get_value(), std::logic_error);
#endif
}


TEST(NosyncResult, UseWithValue)
{
    auto value = "abc"s;
    auto res = make_ok_result(value);
    ASSERT_TRUE(res.is_ok());
    ASSERT_EQ(res.get_value(), value);
#if NOSYNC_TEST_EXCEPTIONS_ENABLED
    ASSERT_THROW(res.get_error(), std::logic_error);
#endif
}


TEST(NosyncResult, UseRvWithValue)
{
    auto value = "abc"s;
    string res_value(make_ok_result(value).get_value());
    ASSERT_EQ(res_value, value);
}


TEST(NosyncResult, CheckValueType)
{
    auto res = make_ok_result("abc"s);
    const auto const_res = make_ok_result("abc"s);

    ASSERT_TRUE((is_same_v<decltype(res.get_value()), string &>));
    ASSERT_TRUE((is_same_v<decltype(const_res.get_value()), const string &>));
    ASSERT_TRUE((is_same_v<decltype(move(res).get_value()), string &&>));
}


TEST(NosyncResult, CheckEqual)
{
    ASSERT_TRUE(make_ok_result("ab"s) == make_ok_result("ab"s));
    ASSERT_FALSE(make_ok_result("ab"s) == make_ok_result("cd"s));
    ASSERT_FALSE(make_ok_result("ab"s) == make_error_result<string>(make_error_code(errc::interrupted)));
    ASSERT_TRUE(
        make_error_result<string>(make_error_code(errc::interrupted))
        == make_error_result<string>(make_error_code(errc::interrupted)));
    ASSERT_FALSE(
        make_error_result<string>(make_error_code(errc::interrupted))
        == make_error_result<string>(make_error_code(errc::timed_out)));
}


TEST(NosyncResult, CheckNotEqual)
{
    ASSERT_FALSE(make_ok_result("ab"s) != make_ok_result("ab"s));
    ASSERT_TRUE(make_ok_result("ab"s) != make_ok_result("cd"s));
    ASSERT_TRUE(make_ok_result("ab"s) != make_error_result<string>(make_error_code(errc::interrupted)));
    ASSERT_FALSE(
        make_error_result<string>(make_error_code(errc::interrupted))
        != make_error_result<string>(make_error_code(errc::interrupted)));
    ASSERT_TRUE(
        make_error_result<string>(make_error_code(errc::interrupted))
        != make_error_result<string>(make_error_code(errc::timed_out)));
}


TEST(NosyncResult, UseVoidWithError)
{
    auto ec = make_error_code(errc::interrupted);
    auto res = make_error_result<void>(ec);
    ASSERT_FALSE(res.is_ok());
    ASSERT_EQ(res.get_error(), ec);
}


TEST(NosyncResult, UseVoidWithOk)
{
    auto res = make_ok_result();
    ASSERT_TRUE(res.is_ok());
#if NOSYNC_TEST_EXCEPTIONS_ENABLED
    ASSERT_THROW(res.get_error(), std::logic_error);
#endif
}


TEST(NosyncResult, CheckVoidEqual)
{
    ASSERT_TRUE(make_ok_result() == make_ok_result());
    ASSERT_FALSE(make_ok_result() == make_error_result<void>(make_error_code(errc::interrupted)));
    ASSERT_TRUE(
        make_error_result<void>(make_error_code(errc::interrupted))
        == make_error_result<void>(make_error_code(errc::interrupted)));
    ASSERT_FALSE(
        make_error_result<void>(make_error_code(errc::interrupted))
        == make_error_result<void>(make_error_code(errc::timed_out)));
}


TEST(NosyncResult, CheckVoidNotEqual)
{
    ASSERT_FALSE(make_ok_result() != make_ok_result());
    ASSERT_TRUE(make_ok_result() != make_error_result<void>(make_error_code(errc::interrupted)));
    ASSERT_FALSE(
        make_error_result<void>(make_error_code(errc::interrupted))
        != make_error_result<void>(make_error_code(errc::interrupted)));
    ASSERT_TRUE(
        make_error_result<void>(make_error_code(errc::interrupted))
        != make_error_result<void>(make_error_code(errc::timed_out)));
}
