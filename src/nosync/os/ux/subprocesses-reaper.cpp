// This file is part of libnosync library. See LICENSE file for license details.
#include <csignal>
#include <nosync/func-request-handler.h>
#include <nosync/raw-error-result.h>
#include <nosync/requests-queue.h>
#include <nosync/type-utils.h>
#include <nosync/os/ux/signal-watcher.h>
#include <nosync/os/ux/subprocesses-reaper.h>
#include <sys/types.h>
#include <sys/wait.h>

using std::make_shared;
using std::move;
using std::shared_ptr;


namespace nosync::os::ux
{

namespace
{

void collect_terminated_children(requests_queue<pid_t, int> &pending_requests)
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

}


result<shared_ptr<request_handler<pid_t, int>>> make_subprocesses_reaper(fd_watching_event_loop &evloop)
{
    auto pending_child_status_requests = make_shared<requests_queue<pid_t, int>>(evloop);

    auto sigchld_watcher_res = make_signal_watcher(
        evloop, SIGCHLD,
        [pending_child_status_requests]() {
            collect_terminated_children(*pending_child_status_requests);
        });

    return sigchld_watcher_res.is_ok()
        ? make_ok_result(
            make_func_request_handler<pid_t, int>(
                [pending_child_status_requests, sigchld_watcher = move(sigchld_watcher_res.get_value())](auto req_pid, auto timeout, auto res_handler) {
                    pending_child_status_requests->push_request(make_copy(req_pid), timeout, move(res_handler));
                }))
        : raw_error_result(sigchld_watcher_res);
}

}
