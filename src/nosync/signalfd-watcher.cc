// This file is part of libnosync library. See LICENSE file for license details.
#include <array>
#include <csignal>
#include <cstring>
#include <nosync/activity-owner.h>
#include <nosync/exceptions.h>
#include <nosync/fd-utils.h>
#include <nosync/shared-fd.h>
#include <nosync/signalfd-watcher.h>
#include <sys/signalfd.h>

using std::array;
using std::function;
using std::make_shared;
using std::move;
using std::shared_ptr;


namespace nosync
{

namespace
{

shared_fd create_signalfd_for_signal(int signal_num)
{
    sigset_t sigset;
    if (sigemptyset(&sigset) < 0 || sigaddset(&sigset, signal_num) < 0) {
        throw_system_error_from_errno();
    }

    int signal_fd = ::signalfd(-1, &sigset, SFD_NONBLOCK | SFD_CLOEXEC);
    if (signal_fd < 0) {
        throw_system_error_from_errno();
    }

    return shared_fd(signal_fd);
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


shared_ptr<interface_type> make_signalfd_watcher(
    fd_watching_event_loop &evloop, int signal_num, function<void()> &&signal_handler)
{
    return make_shared<signalfd_watcher>(
        evloop, create_signalfd_for_signal(signal_num), move(signal_handler));
}

}
