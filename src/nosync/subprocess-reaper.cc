// This file is part of libnosync library. See LICENSE file for license details.
#include <csignal>
#include <nosync/requests-queue.h>
#include <nosync/signalfd-watcher.h>
#include <nosync/subprocess-reaper.h>
#include <nosync/type-utils.h>
#include <sys/types.h>
#include <sys/wait.h>

namespace ch = std::chrono;
using std::make_shared;
using std::move;
using std::shared_ptr;


namespace nosync
{

namespace
{

class subprocess_reaper : public request_handler<pid_t, int>
{
public:
    explicit subprocess_reaper(fd_watching_event_loop &evloop);

    void handle_request(
        pid_t &&req_pid, ch::nanoseconds timeout,
        result_handler<int> &&res_handler) override;

private:
    void collect_terminated_children();

    requests_queue<pid_t, int> pending_requests;
    shared_ptr<interface_type> sigchld_watcher;
};


subprocess_reaper::subprocess_reaper(fd_watching_event_loop &evloop)
    : pending_requests(evloop), sigchld_watcher()
{
    sigchld_watcher = make_signalfd_watcher(
        evloop, SIGCHLD,
        [this]() {
            collect_terminated_children();
        });
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
    return make_shared<subprocess_reaper>(evloop);
}

}
