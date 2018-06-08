// This file is part of libnosync library. See LICENSE file for license details.
#include <gtest/gtest.h>
#include <nosync/string-utils.h>
#include <string>

using namespace std::string_literals;
using nosync::try_decode_hex_string_to_bytes;


TEST(NosyncStringUtils, TryDecodeHexStringToBytes)
{
    auto bytes_empty = try_decode_hex_string_to_bytes(""s);
    ASSERT_TRUE(bytes_empty);
    ASSERT_EQ(*bytes_empty, ""s);

    auto bytes_ok = try_decode_hex_string_to_bytes("01DeadBeef"s);
    ASSERT_TRUE(bytes_ok);
    ASSERT_EQ(*bytes_ok, "\x01\xDE\xAD\xBE\xEF"s);

    auto bytes_odd = try_decode_hex_string_to_bytes("0"s);
    ASSERT_FALSE(bytes_odd);

    auto bytes_bad_chars = try_decode_hex_string_to_bytes("BadHex"s);
    ASSERT_FALSE(bytes_bad_chars);
}
