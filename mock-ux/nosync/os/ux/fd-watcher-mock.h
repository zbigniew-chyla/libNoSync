// This file is part of libnosync library. See LICENSE file for license details.
#ifndef NOSYNC__OS__UX__FD_WATCHER_MOCK_H
#define NOSYNC__OS__UX__FD_WATCHER_MOCK_H

#include <gmock/gmock.h>
#include <nosync/os/ux/fd-watcher.h>


namespace nosync::os::ux
{

class fd_watcher_no_rvref_wrapper : public fd_watcher
{
public:
    fd_watcher_no_rvref_wrapper() = default;

    std::unique_ptr<activity_handle> add_watch(int fd, fd_watch_mode mode, std::function<void()> &&notify_func) override final;

    virtual std::unique_ptr<activity_handle> add_watch_impl(int fd, fd_watch_mode mode, std::function<void()> notify_func) = 0;
};


class fd_watcher_mock : public fd_watcher_no_rvref_wrapper
{
public:
    fd_watcher_mock() = default;

    MOCK_METHOD3(add_watch_impl, std::unique_ptr<activity_handle>(int, fd_watch_mode, std::function<void()>));
};

}

#include <nosync/os/ux/fd-watcher-mock-impl.h>

#endif
