// This file is part of libnosync library. See LICENSE file for license details.
#include <csignal>
#include <nosync/event-loop-utils.h>
#include <nosync/memory-utils.h>
#include <nosync/raw-error-result.h>
#include <nosync/requests-queue.h>
#include <nosync/result-utils.h>
#include <nosync/signalfd-watcher.h>
#include <nosync/subprocess-reaper.h>
#include <nosync/time-utils.h>
#include <nosync/type-utils.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/wait.h>
#include <system_error>

namespace ch = std::chrono;
using std::enable_shared_from_this;
using std::make_shared;
using std::move;
using std::runtime_error;
using std::shared_ptr;


namespace nosync
{

namespace
{

class subprocess_reaper : public request_handler<pid_t, int>, public enable_shared_from_this<subprocess_reaper>
{
public:
    explicit subprocess_reaper(fd_watching_event_loop &evloop);

    void start();

    void handle_request(
        pid_t &&req_pid, ch::nanoseconds timeout,
        result_handler<int> &&res_handler) override;

private:
    void collect_terminated_children();

    fd_watching_event_loop &evloop;
    requests_queue<pid_t, int> pending_requests;
    shared_ptr<interface_type> sigchld_watcher;
};


subprocess_reaper::subprocess_reaper(fd_watching_event_loop &evloop)
    : evloop(evloop), pending_requests(evloop), sigchld_watcher()
{
}


void subprocess_reaper::start()
{
    if (sigchld_watcher) {
        throw runtime_error("subprocess reaper already started");
    }

    sigchld_watcher = make_signalfd_watcher(
        evloop, SIGCHLD,
        make_weak_this_func_proxy(
            this,
            [](auto &self) {
                self.collect_terminated_children();
            }));
}


void subprocess_reaper::collect_terminated_children()
{
    while (true) {
        int wstatus;
        auto waitpid_res = ::waitpid(-1, &wstatus, WNOHANG);
        if (waitpid_res <= 0) {
            break;
        }

        const auto child_pid = waitpid_res;

        bool found_request;
        do {
            found_request = pending_requests.pull_next_matching_request_to_consumer(
                [&](const auto &req) {
                    return req == child_pid;
                },
                [&](auto &&, auto, auto &&res_handler) {
                    res_handler(make_ok_result(wstatus));
                }
            );
        } while (found_request);
    }
}


void subprocess_reaper::handle_request(
    pid_t &&req_pid, ch::nanoseconds timeout,
    result_handler<int> &&res_handler)
{
    pending_requests.push_request(make_copy(req_pid), timeout, move(res_handler));
}

}


shared_ptr<request_handler<pid_t, int>> make_subprocess_reaper(fd_watching_event_loop &evloop)
{
    auto reaper = make_shared<subprocess_reaper>(evloop);
    reaper->start();
    return reaper;
}

}
