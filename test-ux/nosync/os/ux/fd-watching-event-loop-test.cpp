// This file is part of libnosync library. See LICENSE file for license details.
#include <gtest/gtest.h>
#include <memory>
#include <nosync/os/ux/fd-watching-event-loop-mock.h>

using nosync::os::ux::fd_watching_event_loop_mock;
using std::make_unique;


TEST(NosyncFdWatchingEventLoop, CreateMockInstance)
{
    make_unique<fd_watching_event_loop_mock>();
}
