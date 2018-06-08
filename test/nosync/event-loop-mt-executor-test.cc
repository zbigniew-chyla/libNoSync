// This file is part of libnosync library. See LICENSE file for license details.
#include <atomic>
#include <gtest/gtest.h>
#include <nosync/event-loop-mt-executor.h>
#include <nosync/ppoll-based-event-loop.h>
#include <thread>
#include <utility>
#include <vector>

using namespace std::chrono_literals;
using std::atomic_uint;
using nosync::make_event_loop_mt_executor;
using nosync::make_ppoll_based_event_loop;
using std::make_shared;
using std::move;
using std::thread;
using std::vector;


namespace
{

constexpr auto small_time_increment = 1ns;

}


TEST(NosyncEventLoopMtExecutor, CheckLifetime)
{
    auto evloop = make_ppoll_based_event_loop();

    auto executor_res = make_event_loop_mt_executor(*evloop);
    ASSERT_TRUE(executor_res.is_ok());
    auto executor = move(executor_res.get_value());
    executor = nullptr;

    evloop->run_iterations();
}


TEST(NosyncEventLoopMtExecutor, TestPushFromEvloopThread)
{
    auto evloop = make_ppoll_based_event_loop();

    auto counter = make_shared<unsigned>(0);

    auto executor_res = make_event_loop_mt_executor(*evloop);
    ASSERT_TRUE(executor_res.is_ok());
    auto executor = move(executor_res.get_value());

    evloop->invoke_at(
        evloop->get_etime(),
        [executor, counter]() {
            executor(
                [counter]() {
                    ++*counter;
                });
        });
    executor = nullptr;

    ASSERT_EQ(*counter, 0U);

    evloop->run_iterations();

    ASSERT_EQ(*counter, 1U);
}


TEST(NosyncEventLoopMtExecutor, TestPushFromOtherThreads)
{
    constexpr auto threads_count = 4U;

    auto evloop = make_ppoll_based_event_loop();

    const auto main_thread_id = std::this_thread::get_id();

    auto main_thread_ticks = make_shared<atomic_uint>(0);

    vector<thread> worker_threads;

    auto executor_res = make_event_loop_mt_executor(*evloop);
    ASSERT_TRUE(executor_res.is_ok());
    auto executor = move(executor_res.get_value());

    for (unsigned i = 0; i < threads_count; ++i) {
        evloop->invoke_at(
            evloop->get_etime() + small_time_increment * i,
            [&worker_threads, executor, main_thread_id, main_thread_ticks]() {
                worker_threads.emplace_back(
                    [executor, main_thread_id, main_thread_ticks]() {
                        executor(
                            [main_thread_id, main_thread_ticks]() {
                                if (std::this_thread::get_id() == main_thread_id) {
                                    ++*main_thread_ticks;
                                }
                            });
                    });
            });
    }
    executor = nullptr;

    evloop->run_iterations();

    for (auto &thread : worker_threads) {
        thread.join();
    }

    ASSERT_EQ(*main_thread_ticks, threads_count);
}
