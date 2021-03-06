// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__REQUESTS_QUEUE_H
#define NOSYNC__REQUESTS_QUEUE_H

#include <cstddef>
#include <deque>
#include <functional>
#include <nosync/activity-owner.h>
#include <nosync/eclock.h>
#include <nosync/event-loop.h>
#include <nosync/result-handler.h>
#include <memory>
#include <optional>
#include <string>
#include <tuple>


namespace nosync
{

/*!
Helper class template for implementing queues of async requests.

This class template is to be used by request_handler implementations and similar
classes which must store incoming requests in a queue and use them later in the
same order e.g. to ensure sequential processing, throttling, etc.

Besides just storing the requests, the class automatically handles timeouts for
them. If timeout is reached for a request, timeout error is passed to its
response handler and the request is removed from the queue.
*/
template<typename Req, typename Res>
class requests_queue
{
public:
    explicit requests_queue(event_loop &evloop);
    ~requests_queue();

    requests_queue(requests_queue &&) = default;

    requests_queue &operator=(requests_queue &&) = delete;

    requests_queue(const requests_queue &) = delete;
    requests_queue &operator=(const requests_queue &) = delete;

    void push_request(
        Req &&request, eclock::time_point timeout_end,
        result_handler<Res> &&res_handler);

    void push_request(
        Req &&request, eclock::duration timeout,
        result_handler<Res> &&res_handler);

    bool has_requests() const;

    template<typename F>
    bool pull_next_request_to_consumer(const F &req_consumer);

    template<typename RequestPredicate, typename Consumer>
    bool pull_next_matching_request_to_consumer(
        const RequestPredicate &predicate, const Consumer &req_consumer);

    template<typename Func>
    void for_each_request(const Func &func) const;

private:
    void handle_pending_timeouts();
    void disable_timeout_task_if_present();
    void reschedule_timeout_task();

    event_loop &evloop;
    std::deque<std::tuple<Req, eclock::time_point, result_handler<Res>>> requests;
    std::optional<std::tuple<eclock::time_point, activity_owner>> scheduled_timeout_task;
};

}

#include <nosync/requests-queue-impl.h>

#endif
