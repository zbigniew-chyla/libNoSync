// This file is part of libnosync library. See LICENSE file for license details.
#include <nosync/hex-encoded-messages-request-handler.h>
#include <nosync/raw-error-result.h>
#include <nosync/string-utils.h>
#include <nosync/transforming-request-handler.h>
#include <system_error>
#include <utility>

using std::errc;
using std::move;
using std::shared_ptr;
using std::string;


namespace nosync
{

shared_ptr<request_handler<string, string>> make_hex_encoded_messages_request_handler(
    event_loop &evloop, shared_ptr<request_handler<string, string>> &&raw_messages_req_handler)
{
    return make_full_transforming_request_handler<string, string, string, string>(
        evloop, move(raw_messages_req_handler),
        [](auto msg) {
            return make_ok_result(bytes_to_hex_string(msg));
        },
        [](auto msg) {
            auto decoded_msg = try_decode_hex_string_to_bytes(msg);
            return decoded_msg ? make_ok_result(move(*decoded_msg)) : raw_error_result(errc::bad_message);
        });
}

}
