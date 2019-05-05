// This file is part of libnosync library. See LICENSE file for license details.
#include <algorithm>
#include <experimental/array>
#include <functional>
#include <gtest/gtest.h>
#include <memory>
#include <nosync/os/ux/manual-fd-watcher.h>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

using nosync::os::ux::fd_watch_mode;
using nosync::activity_handle;
using nosync::os::ux::manual_fd_watcher;
using std::experimental::make_array;
using std::function;
using std::get;
using std::make_shared;
using std::make_tuple;
using std::move;
using std::set;
using std::shared_ptr;
using std::tuple;
using std::unique_ptr;
using std::vector;


namespace
{

class destroy_notifier
{
public:
    explicit destroy_notifier(function<void()> &&notify_func);
    ~destroy_notifier();

    destroy_notifier(const destroy_notifier &notify_func) = default;
    destroy_notifier(destroy_notifier &&notify_func) = default;
    destroy_notifier &operator=(const destroy_notifier &notify_func) = default;
    destroy_notifier &operator=(destroy_notifier &&notify_func) = default;

private:
    shared_ptr<function<void()>> notify_func;
};


destroy_notifier::destroy_notifier(function<void()> &&notify_func)
    : notify_func(make_shared<function<void()>>(move(notify_func)))
{
}


destroy_notifier::~destroy_notifier()
{
    if (notify_func) {
        (*notify_func)();
    }
}

}


TEST(NosyncManualFdWatcher, TestInitial)
{
    auto watcher = make_shared<manual_fd_watcher>();

    watcher->notify_watches(123, fd_watch_mode::input);
    watcher->notify_watches(456, fd_watch_mode::output);

    const auto fd_info = watcher->get_watches_fd_info();
    ASSERT_TRUE(fd_info.empty());
}


TEST(NosyncManualFdWatcher, TestSingleWatch)
{
    auto watcher = make_shared<manual_fd_watcher>();

    auto counter = 0U;

    auto handle = watcher->add_watch(
        123, fd_watch_mode::input,
        [&]() {
            ++counter;
        });
    ASSERT_TRUE(handle->is_enabled());

    ASSERT_EQ(counter, 0U);

    watcher->notify_watches(123, fd_watch_mode::output);
    ASSERT_EQ(counter, 0U);

    watcher->notify_watches(456, fd_watch_mode::input);
    ASSERT_EQ(counter, 0U);

    watcher->notify_watches(123, fd_watch_mode::input);
    ASSERT_EQ(counter, 1U);

    watcher->notify_watches(123, fd_watch_mode::input);
    ASSERT_EQ(counter, 2U);

    handle->disable();
    ASSERT_FALSE(handle->is_enabled());

    watcher->notify_watches(123, fd_watch_mode::input);
    ASSERT_EQ(counter, 2U);
}


TEST(NosyncManualFdWatcher, TestCheckWatchesFdInfo)
{
    auto watcher = make_shared<manual_fd_watcher>();

    auto empty_func = []() {
    };

    watcher->add_watch(789, fd_watch_mode::output, empty_func);
    watcher->add_watch(123, fd_watch_mode::input, empty_func);
    watcher->add_watch(456, fd_watch_mode::input, empty_func);
    watcher->add_watch(789, fd_watch_mode::output, empty_func);
    watcher->add_watch(123, fd_watch_mode::input, empty_func);

    auto fd_info = watcher->get_watches_fd_info();
    auto fd_info_set = set<tuple<int, fd_watch_mode>>(fd_info.cbegin(), fd_info.cend());

    const decltype(fd_info_set) expected_result = {
        {123, fd_watch_mode::input},
        {456, fd_watch_mode::input},
        {789, fd_watch_mode::output},
    };

    ASSERT_EQ(fd_info.size(), fd_info_set.size());
    ASSERT_EQ(fd_info_set, expected_result);
}


TEST(NosyncManualFdWatcher, TestMultipleWatchesSingleCond)
{
    constexpr auto watch_count = 10U;
    constexpr auto fd = 123;
    constexpr auto mode = fd_watch_mode::input;

    auto watcher = make_shared<manual_fd_watcher>();

    vector<unsigned> counters;
    vector<unique_ptr<activity_handle>> watch_handles;
    for (unsigned i = 0; i < watch_count; ++i) {
        counters.push_back(0U);
        watch_handles.push_back(
            watcher->add_watch(
                fd, mode,
                [&counters, i]() {
                    ++counters[i];
                }));
    }

    watcher->notify_watches(123, fd_watch_mode::output);
    for (unsigned i = 0; i < watch_count; ++i) {
        ASSERT_TRUE(watch_handles[i]->is_enabled());
        ASSERT_EQ(counters[i], 0U);
    }

    watcher->notify_watches(123, fd_watch_mode::input);
    for (unsigned i = 0; i < watch_count; ++i) {
        ASSERT_EQ(counters[i], 1U);
    }

    for (unsigned i = 0; i < watch_count; ++i) {
        if (i % 2 == 0) {
            watch_handles[i]->disable();
            ASSERT_FALSE(watch_handles[i]->is_enabled());
            watch_handles[i].reset();
        }
    }

    watcher->notify_watches(123, fd_watch_mode::input);
    for (unsigned i = 0; i < watch_count; ++i) {
        if (i % 2 == 0) {
            ASSERT_EQ(counters[i], 1U);
        } else {
            ASSERT_EQ(counters[i], 2U);
        }
    }
}


TEST(NosyncManualFdWatcher, TestMultipleWatchesMultipleCond)
{
    const auto watch_keys = make_array(
        make_tuple(123, fd_watch_mode::input),
        make_tuple(456, fd_watch_mode::input),
        make_tuple(123, fd_watch_mode::output),
        make_tuple(789, fd_watch_mode::input));

    auto watcher = make_shared<manual_fd_watcher>();

    vector<unsigned> counters;
    vector<unique_ptr<activity_handle>> watch_handles;
    for (unsigned i = 0; i < watch_keys.size(); ++i) {
        counters.push_back(0U);
        watch_handles.push_back(
            watcher->add_watch(
                get<int>(watch_keys[i]),
                get<fd_watch_mode>(watch_keys[i]),
                [&counters, i]() {
                    ++counters[i];
                }));
    }

    for (unsigned i = 0; i < watch_keys.size(); ++i) {
        ASSERT_TRUE(watch_handles[i]->is_enabled());
        ASSERT_EQ(counters[i], 0U);
        watcher->notify_watches(get<int>(watch_keys[i]), get<fd_watch_mode>(watch_keys[i]));
        ASSERT_EQ(counters[i], 1U);
    }

    for (unsigned i = 0; i < watch_keys.size(); ++i) {
        watch_handles[i]->disable();
        ASSERT_FALSE(watch_handles[i]->is_enabled());
        watcher->notify_watches(get<int>(watch_keys[i]), get<fd_watch_mode>(watch_keys[i]));
        ASSERT_EQ(counters[i], 1U);
    }
}


TEST(NosyncManualFdWatcher, TestDetachedHandle)
{
    auto watcher = make_shared<manual_fd_watcher>();

    auto handle = watcher->add_watch(
        123, fd_watch_mode::input,
        []() {
        });
    ASSERT_TRUE(handle->is_enabled());

    watcher.reset();
    ASSERT_FALSE(handle->is_enabled());
}


TEST(NosyncManualFdWatcher, TestRemoveDuringNotify)
{
    constexpr auto fd = 123;
    constexpr auto mode = fd_watch_mode::input;

    auto watcher = make_shared<manual_fd_watcher>();

    auto counter_a = 0U;
    auto counter_b = 0U;

    unique_ptr<activity_handle> a_handle;
    unique_ptr<activity_handle> b_handle;

    a_handle = watcher->add_watch(
        fd, mode,
        [&]() {
            ++counter_a;
            b_handle->disable();
        });
    b_handle = watcher->add_watch(
        fd, mode,
        [&]() {
            ++counter_b;
        });

    watcher->notify_watches(fd, mode);
    ASSERT_EQ(counter_a, 1U);
    ASSERT_EQ(counter_b, 0U);
    ASSERT_TRUE(a_handle->is_enabled());
    ASSERT_FALSE(b_handle->is_enabled());
}


TEST(NosyncManualFdWatcher, TestAddDuringNotify)
{
    constexpr auto fd = 123;
    constexpr auto mode = fd_watch_mode::input;

    auto watcher = make_shared<manual_fd_watcher>();

    auto counter_a = 0U;
    auto counter_b = 0U;

    unique_ptr<activity_handle> a_handle;
    unique_ptr<activity_handle> b_handle;

    a_handle = watcher->add_watch(
        fd, mode,
        [&]() {
            if (counter_a == 0) {
                b_handle = watcher->add_watch(
                    fd, mode,
                    [&]() {
                        ++counter_b;
                    });
            }
            ++counter_a;
        });

    ASSERT_TRUE(a_handle);
    ASSERT_TRUE(a_handle->is_enabled());
    ASSERT_FALSE(b_handle);

    watcher->notify_watches(fd, mode);
    ASSERT_EQ(counter_a, 1U);
    ASSERT_EQ(counter_b, 0U);
    ASSERT_TRUE(a_handle);
    ASSERT_TRUE(a_handle->is_enabled());
    ASSERT_TRUE(b_handle);
    ASSERT_TRUE(b_handle->is_enabled());

    watcher->notify_watches(fd, mode);
    ASSERT_EQ(counter_a, 2U);
    ASSERT_EQ(counter_b, 1U);
}


TEST(NosyncManualFdWatcher, TestUseHandleDuringDestroy)
{
    constexpr auto mode = fd_watch_mode::input;

    unique_ptr<activity_handle> a_handle;
    unique_ptr<activity_handle> b_handle;

    auto watcher = make_shared<manual_fd_watcher>();

    auto a_watch_destroy_checker = [&]() {
        ASSERT_TRUE(a_handle);
        ASSERT_FALSE(a_handle->is_enabled());
    };
    auto b_watch_destroy_checker = [&]() {
        ASSERT_TRUE(b_handle);
        ASSERT_FALSE(b_handle->is_enabled());
    };

    a_handle = watcher->add_watch(
        101, mode,
        [checker = destroy_notifier(move(a_watch_destroy_checker))]() {
        });
    b_handle = watcher->add_watch(
        100, mode,
        [checker = destroy_notifier(move(b_watch_destroy_checker))]() {
        });

    ASSERT_TRUE(a_handle);
    ASSERT_TRUE(a_handle->is_enabled());
    ASSERT_TRUE(b_handle);
    ASSERT_TRUE(b_handle->is_enabled());

    watcher.reset();

    ASSERT_TRUE(a_handle);
    ASSERT_FALSE(a_handle->is_enabled());
    ASSERT_TRUE(b_handle);
    ASSERT_FALSE(b_handle->is_enabled());
}
