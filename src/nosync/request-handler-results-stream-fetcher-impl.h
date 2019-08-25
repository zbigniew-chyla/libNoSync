// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__REQUEST_HANDLER_RESULTS_STREAM_FETCHER_IMPL_H
#define NOSYNC__REQUEST_HANDLER_RESULTS_STREAM_FETCHER_IMPL_H

#include <cstddef>
#include <memory>
#include <nosync/eclock.h>
#include <nosync/request-handler.h>
#include <utility>


namespace nosync
{

namespace request_handler_results_stream_fetcher_impl
{

template<typename Res>
class request_handler_results_stream_fetcher : public std::enable_shared_from_this<request_handler_results_stream_fetcher<Res>>
{
public:
    request_handler_results_stream_fetcher(
        std::shared_ptr<request_handler<std::nullptr_t, Res>> &&req_handler,
        std::function<iteration_status(result<Res>)> &&results_handler,
        eclock::duration req_timeout);

    void fetch_next_result();

private:
    std::shared_ptr<request_handler<std::nullptr_t, Res>> req_handler;
    std::function<iteration_status(result<Res>)> results_handler;
    eclock::duration req_timeout;
};


template<typename Res>
request_handler_results_stream_fetcher<Res>::request_handler_results_stream_fetcher(
    std::shared_ptr<request_handler<std::nullptr_t, Res>> &&req_handler,
    std::function<iteration_status(result<Res>)> &&results_handler,
    eclock::duration req_timeout)
    : req_handler(std::move(req_handler)), results_handler(std::move(results_handler)),
    req_timeout(req_timeout)
{
}


template<typename Res>
void request_handler_results_stream_fetcher<Res>::fetch_next_result()
{
    req_handler->handle_request(
        nullptr, req_timeout,
        [self_wptr = this->weak_from_this()](auto res) {
            auto self = self_wptr.lock();
            if (self) {
                auto iter_status = self->results_handler(std::move(res));
                if (iter_status == iteration_status::running) {
                    self->fetch_next_result();
                } else {
                    self->req_handler.reset();
                    self->results_handler = nullptr;
                }
            }
        });
}

}


template<typename Res>
std::shared_ptr<void> make_request_handler_results_stream_fetcher(
    std::shared_ptr<request_handler<std::nullptr_t, Res>> &&req_handler,
    eclock::duration req_timeout, std::function<iteration_status(result<Res>)> &&results_handler)
{
    using namespace request_handler_results_stream_fetcher_impl;
    using std::move;

    auto fetcher = std::make_shared<request_handler_results_stream_fetcher<Res>>(
        move(req_handler), move(results_handler), req_timeout);
    fetcher->fetch_next_result();
    return fetcher;
}


template<typename Res>
std::shared_ptr<void> make_request_handler_results_stream_fetcher(
    std::shared_ptr<request_handler<std::nullptr_t, Res>> &&req_handler,
    eclock::duration req_timeout, std::function<void(Res)> &&results_values_consumer)
{
    using std::move;

    return make_request_handler_results_stream_fetcher<Res>(
        move(req_handler), req_timeout,
        [results_values_consumer = move(results_values_consumer)](result<Res> res) {
            if (res.is_ok()) {
                results_values_consumer(move(res.get_value()));
                return iteration_status::running;
            } else {
                return iteration_status::stopped;
            }
        });
}

}

#endif
