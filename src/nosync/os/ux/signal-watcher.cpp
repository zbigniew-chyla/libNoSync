// This file is part of libnosync library. See LICENSE file for license details.
#include <array>
#include <csignal>
#include <cstring>
#include <nosync/activity-owner.h>
#include <nosync/result-utils.h>
#include <nosync/os/ux/fd-utils.h>
#include <nosync/os/ux/shared-fd.h>
#include <nosync/os/ux/signal-watcher.h>
#include <sys/signalfd.h>

using std::array;
using std::function;
using std::make_shared;
using std::move;
using std::shared_ptr;


namespace nosync::os::ux
{

namespace
{

result<shared_fd> create_signalfd_for_signal(int signal_num)
{
    sigset_t sigset;
    if (sigemptyset(&sigset) < 0 || sigaddset(&sigset, signal_num) < 0) {
        return make_raw_error_result_from_errno();
    }

    int signalfd_retval = ::signalfd(-1, &sigset, SFD_NONBLOCK | SFD_CLOEXEC);
    return signalfd_retval >= 0
        ? make_ok_result(shared_fd(signalfd_retval))
        : make_raw_error_result_from_errno();
}


class signalfd_watcher : public interface_type
{
public:
    signalfd_watcher(fd_watching_event_loop &evloop, shared_fd &&signal_fd, function<void()> &&signal_handler);

private:
    void handle_signalfd_input();

    function<void()> signal_handler;
    shared_fd signal_fd;
    activity_owner signalfd_watch_owner;
};


signalfd_watcher::signalfd_watcher(
    fd_watching_event_loop &evloop, shared_fd &&signal_fd, function<void()> &&signal_handler)
    : signal_handler(move(signal_handler)), signal_fd(move(signal_fd))
{
    signalfd_watch_owner = evloop.add_watch(
        *this->signal_fd, fd_watch_mode::input,
        [this]() {
            handle_signalfd_input();
        });
}


void signalfd_watcher::handle_signalfd_input()
{
    array<char, sizeof(::signalfd_siginfo)> siginfo_data;
    auto read_res = read_some_bytes_from_fd(*signal_fd, siginfo_data.data(), siginfo_data.size());
    if (read_res.is_ok() && read_res.get_value() == siginfo_data.size()) {
        signal_handler();
    }
}

}


result<shared_ptr<interface_type>> make_signal_watcher(
    fd_watching_event_loop &evloop, int signal_num, function<void()> &&signal_handler)
{
    auto signal_fd_res = create_signalfd_for_signal(signal_num);
    return signal_fd_res.is_ok()
        ? make_ok_result(
            make_shared<signalfd_watcher>(
                evloop, move(signal_fd_res.get_value()), move(signal_handler)))
        : raw_error_result(signal_fd_res);
}

}
