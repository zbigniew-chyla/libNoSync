// This file is part of libnosync library. See LICENSE file for license details.
#include <algorithm>
#include <nosync/eclock.h>
#include <nosync/input-messages-dispatch-handler.h>
#include <nosync/memory-utils.h>
#include <nosync/raw-error-result.h>
#include <nosync/requests-queue.h>
#include <nosync/result-utils.h>
#include <nosync/time-utils.h>
#include <nosync/utils.h>
#include <optional>
#include <string_view>
#include <system_error>

namespace ch = std::chrono;
using namespace std::chrono_literals;
using std::enable_shared_from_this;
using std::errc;
using std::optional;
using std::function;
using std::make_shared;
using std::move;
using std::nullptr_t;
using std::shared_ptr;
using std::string;
using std::string_view;


namespace nosync
{

namespace
{

class input_messages_dispatch_handler : public request_handler<string, string>, public enable_shared_from_this<input_messages_dispatch_handler>
{
public:
    input_messages_dispatch_handler(
        event_loop &evloop, shared_ptr<request_handler<nullptr_t, string>> &&messages_reader,
        function<optional<string>(string_view)> &&message_id_decoder);

    void handle_request(
        string &&msg_id, ch::nanoseconds timeout,
        result_handler<string> &&res_handler) override;

private:
    void read_next_message_if_needed();

    shared_ptr<request_handler<nullptr_t, string>> messages_reader;
    function<optional<string>(string_view)> message_id_decoder;
    bool read_ongoing;
    requests_queue<string, string> pending_requests;
    requests_queue<string, string> new_pending_requests;
};


input_messages_dispatch_handler::input_messages_dispatch_handler(
    event_loop &evloop, shared_ptr<request_handler<nullptr_t, string>> &&messages_reader,
    function<optional<string>(string_view)> &&message_id_decoder)
    : messages_reader(move(messages_reader)), message_id_decoder(move(message_id_decoder)),
    read_ongoing(false), pending_requests(evloop), new_pending_requests(evloop)
{
}


void input_messages_dispatch_handler::handle_request(
    string &&msg_id, ch::nanoseconds timeout, result_handler<string> &&res_handler)
{
    if (!read_ongoing) {
        pending_requests.push_request(move(msg_id), timeout, move(res_handler));
        read_next_message_if_needed();
    } else {
        new_pending_requests.push_request(move(msg_id), timeout, move(res_handler));
    }
}


void input_messages_dispatch_handler::read_next_message_if_needed()
{
    if (!pending_requests.has_requests() || read_ongoing) {
        return;
    }

    auto min_timeout = ch::nanoseconds::max();
    pending_requests.for_each_request(
        [&](const auto &, auto timeout, const auto &) {
            min_timeout = std::min(min_timeout, timeout);
        });

    messages_reader->handle_request(
        nullptr, min_timeout,
        [disp_handler_wptr = weak_from_that(this)](auto read_res) {
            auto disp_handler_ptr = disp_handler_wptr.lock();
            if (!disp_handler_ptr) {
                return;
            }

            disp_handler_ptr->read_ongoing = false;

            while (disp_handler_ptr->new_pending_requests.has_requests()) {
                disp_handler_ptr->new_pending_requests.pull_next_request_to_consumer(
                    [&](auto req, auto timeout, auto res_handler) {
                        disp_handler_ptr->pending_requests.push_request(move(req), timeout, move(res_handler));
                    });
            }

            optional<result_handler<string>> res_handler;
            if (read_res.is_ok()) {
                auto read_msg_id = disp_handler_ptr->message_id_decoder(read_res.get_value());
                if (read_msg_id) {
                    disp_handler_ptr->pending_requests.pull_next_matching_request_to_consumer(
                        [&](auto &req) {
                            return req == read_msg_id;
                        },
                        [&](auto &&, auto, auto &&res_handler) {
                            res_handler(move(read_res));
                        });
                }
            } else {
                if (disp_handler_ptr->pending_requests.has_requests() && read_res.get_error() != make_error_code(errc::timed_out)) {
                    disp_handler_ptr->pending_requests.pull_next_request_to_consumer(
                        [&](auto &&, auto, auto &&res_handler) {
                            res_handler(move(read_res));
                        });
                }
            }

            disp_handler_ptr->read_next_message_if_needed();
        });
    read_ongoing = true;
}

}


shared_ptr<request_handler<string, string>> make_input_messages_dispatch_handler(
    event_loop &evloop, shared_ptr<request_handler<nullptr_t, string>> &&messages_reader,
    function<optional<string>(string_view)> &&message_id_decoder)
{
    return make_shared<input_messages_dispatch_handler>(
        evloop, move(messages_reader), move(message_id_decoder));
}

}
