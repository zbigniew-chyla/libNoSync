#include <nosync/request-handler-semaphore.h>
#include <utility>

using std::move;
using std::make_shared;
using std::shared_ptr;


namespace nosync
{

request_handler_semaphore::request_handler_semaphore(event_loop &evloop, unsigned init_value)
    : evloop(evloop), sem_value(init_value), pending_requests(evloop)
{
}


void request_handler_semaphore::handle_release()
{
    if (pending_requests.has_requests()) {
        pending_requests.pull_next_request_to_consumer(
            [](auto request_func, auto timeout, auto res_handler) {
                request_func(
                    timeout,
                    [res_handler = move(res_handler)]() {
                        res_handler(make_ok_result());
                    });
            });
    } else {
        ++sem_value;
    }
}


shared_ptr<request_handler_semaphore> make_request_handler_semaphore(event_loop &evloop, unsigned init_value)
{
    class request_handler_semaphore_impl : public request_handler_semaphore
    {
    public:
        request_handler_semaphore_impl(event_loop &evloop, unsigned init_value)
            : request_handler_semaphore(evloop, init_value)
        {
        }
    };

    return make_shared<request_handler_semaphore_impl>(evloop, init_value);
}

}
