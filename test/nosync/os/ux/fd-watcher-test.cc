// This file is part of libnosync library. See LICENSE file for license details.
#include <gtest/gtest.h>
#include <memory>
#include <nosync/os/ux/fd-watcher-mock.h>

using nosync::os::ux::fd_watcher_mock;
using std::make_unique;


TEST(NosyncFdWatcher, CreateMockInstance)
{
    make_unique<fd_watcher_mock>();
}
