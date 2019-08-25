// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__REQUEST_HANDLER_RESULTS_STREAM_FETCHER_H
#define NOSYNC__REQUEST_HANDLER_RESULTS_STREAM_FETCHER_H

#include <cstddef>
#include <functional>
#include <memory>
#include <nosync/iteration-status.h>
#include <nosync/request-handler.h>


namespace nosync
{

/*!
Create an object that continuously (but sequentually) fetches results from request_handler<nullptr_t, ...>.

The function takes a request_handler<> accepting nullptr_t requests and creates an object that
keeps calling handle_request() on it and passes a result from each call to results_handler
function. The handle_request() calls are made sequentially, i.e. another call is made after
receiving a result from preceding one). Successive handle_request() call is made only if
results_handler for the previous one returns iteration_status::running.

The fetcher calls handle_request() with timeout set to the maximum value. If other timeout is
desired, req_handler should be decorated with the appropriate "timeout limiter".

Immediately after receiving iteration_status::stopped from results_handler both request_handler<>
and results_handler are destroyed. Both are also destroyed (if still alive at that point) when an
object returned by this function is destroyed.
*/
template<typename Res>
std::shared_ptr<void> make_request_handler_results_stream_fetcher(
    std::shared_ptr<request_handler<std::nullptr_t, Res>> &&req_handler,
    std::function<iteration_status(result<Res>)> &&results_handler);


/*!
Create an object that continuously (but sequentually) fetches results' values from request_handler<nullptr_t, ...>.

This is simple wrapper around the "full" variant of the function. It produces a fetcher that stops
calling handle_request() on request_handler<> object after receiving any error result. As a
consequence, it accepts simple consumer of Res values instead of more complex function accepting
result<Res> objects and returning iteration_status.

Immediately after receiving error result from handle_request() both request_handler<> and
results_handler are destroyed.
*/
template<typename Res>
std::shared_ptr<void> make_request_handler_results_stream_fetcher(
    std::shared_ptr<request_handler<std::nullptr_t, Res>> &&req_handler,
    std::function<void(Res)> &&results_values_consumer);

}

#include <nosync/request-handler-results-stream-fetcher-impl.h>

#endif
