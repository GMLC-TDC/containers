/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "gtest/gtest.h"
#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <utility>
/** these test cases test data_block and data_view objects
 */

#include "BlockingPriorityQueue.hpp"
using gmlc::containers::BlockingPriorityQueue;

/** test basic operations */
TEST(blocking_priority_queue, basic_tests)
{
    BlockingPriorityQueue<int> queue;

    queue.push(45);
    queue.push(54);

    EXPECT_FALSE(queue.empty());

    auto popped_value = queue.try_pop();
    EXPECT_EQ(*popped_value, 45);
    popped_value = queue.try_pop();
    EXPECT_EQ(*popped_value, 54);

    popped_value = queue.try_pop();
    EXPECT_FALSE(popped_value);
    EXPECT_TRUE(queue.empty());

    queue.push(45);
    queue.push(54);

    // check the prioritization is working
    queue.pushPriority(65);

    popped_value = queue.try_pop();
    EXPECT_EQ(*popped_value, 65);
    popped_value = queue.try_pop();
    EXPECT_EQ(*popped_value, 45);
}

/** test with a move only element*/
TEST(blocking_priority_queue, move_only_tests)
{
    BlockingPriorityQueue<std::unique_ptr<double>> queue;

    queue.push(std::make_unique<double>(4534.23));

    auto second_element = std::make_unique<double>(34.234);
    queue.push(std::move(second_element));

    EXPECT_FALSE(queue.empty());

    auto popped_value = queue.try_pop();
    EXPECT_EQ(**popped_value, 4534.23);
    popped_value = queue.try_pop();
    EXPECT_EQ(**popped_value, 34.234);
    second_element = std::make_unique<double>(29.785);
    queue.pushPriority(std::move(second_element));

    popped_value = queue.try_pop();
    EXPECT_EQ(**popped_value, 29.785);
    popped_value = queue.try_pop();
    EXPECT_FALSE(popped_value);
    EXPECT_TRUE(queue.empty());
}

/** test the ordering with a larger number of inputs*/
TEST(blocking_priority_queue, ordering_tests)
{
    BlockingPriorityQueue<int> queue;

    for (int index = 1; index < 10; ++index) {
        queue.push(index);
    }

    auto popped_value = queue.try_pop();
    EXPECT_EQ(*popped_value, 1);
    for (int index = 2; index < 7; ++index) {
        popped_value = queue.try_pop();
        EXPECT_EQ(*popped_value, index);
    }
    for (int index = 10; index < 20; ++index) {
        queue.push(index);
    }
    queue.pushPriority(99);
    popped_value = queue.try_pop();
    EXPECT_EQ(*popped_value, 99);
    for (int index = 7; index < 20; ++index) {
        popped_value = queue.try_pop();
        EXPECT_EQ(*popped_value, index);
    }

    EXPECT_TRUE(queue.empty());
}

TEST(blocking_priority_queue, emplace_tests)
{
    BlockingPriorityQueue<std::pair<int, double>> queue;

    queue.emplace(10, 45.4);
    queue.emplace(11, 34.1);
    queue.emplace(12, 34.2);
    queue.emplacePriority(14, 19.99);

    auto popped_value = queue.try_pop();
    EXPECT_EQ(popped_value->first, 14);
    EXPECT_EQ(popped_value->second, 19.99);

    popped_value = queue.try_pop();
    EXPECT_EQ(popped_value->first, 10);
    EXPECT_EQ(popped_value->second, 45.4);
    popped_value = queue.try_pop();
    EXPECT_EQ(popped_value->first, 11);
    EXPECT_EQ(popped_value->second, 34.1);
}

TEST(blocking_priority_queue, clear_tests)
{
    BlockingPriorityQueue<int64_t> queue;
    queue.push(10);
    queue.push(100);
    queue.push(1000);
    queue.pop();
    queue.push(20);
    queue.push(20);
    queue.pushPriority(9);
    queue.pushPriority(18);
    EXPECT_FALSE(queue.empty());
    queue.clear();
    EXPECT_TRUE(queue.empty());
}

TEST(blocking_priority_queue, multithreaded_tests_wait)
{
    BlockingPriorityQueue<std::pair<int64_t, int64_t>> queue;
    auto first_thread_body = [&queue]() {
        int iteration = 0;
        std::random_device device;
        std::mt19937 generator(device());
        std::uniform_int_distribution<std::mt19937::result_type> distribution(
            1, 10);
        const std::pair<int64_t, int64_t> element{10, 10};
        while (iteration < 500) {
            ++iteration;
            auto result = queue.pop(std::chrono::milliseconds(10));

            switch (distribution(generator)) {
                case 1:
                    break;
                case 2:
                    queue.pushPriority(element);
                    break;
                case 3:
                    if (result) {
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(10));
                    }
                    [[fallthrough]];
                default:
                    queue.push(element);
                    break;
            }
        }
    };
    auto second_thread_body = [&queue]() {
        int iteration = 0;
        std::random_device device;
        std::mt19937 generator(device());
        std::uniform_int_distribution<std::mt19937::result_type> distribution(
            1, 10);
        while (iteration < 500) {
            ++iteration;
            auto result = queue.pop();
            static_cast<void>(result);
            switch (distribution(generator)) {
                case 1:
                    break;
                case 2:
                    queue.emplacePriority(20, 20);
                    break;
                case 3:
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                    [[fallthrough]];
                default:
                    queue.emplace(30, 30);
                    break;
            }
        }
    };

    auto third_thread_body = [&queue]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1200));
        int iteration = 0;
        while (iteration++ < 500) {
            queue.emplace(80, 80);
        }
    };

    auto first_thread = std::thread(first_thread_body);
    auto second_thread = std::thread(second_thread_body);
    auto third_thread = std::thread(third_thread_body);
    EXPECT_TRUE(true);
    first_thread.join();
    second_thread.join();
    third_thread.join();
}

/** test with single consumer/single producer*/
TEST(blocking_priority_queue, multithreaded_tests)
{
    BlockingPriorityQueue<int64_t> queue(1010000);

    for (int64_t index = 0; index < 10'000; ++index) {
        queue.push(index);
    }
    auto producer = [&]() {
        for (int64_t index = 10'000; index < 1'010'000; ++index) {
            queue.push(index);
        }
    };

    auto consumer = [&]() {
        auto result = queue.try_pop();
        int64_t count = 0;
        while (result) {
            ++count;
            result = queue.try_pop();
            if (!result) {  // make an additional sleep period so the producer
                            // can catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                result = queue.try_pop();
            }
        }
        return count;
    };

    auto producer_thread = std::thread(producer);

    auto consumed_count = consumer();
    EXPECT_EQ(consumed_count, 1'010'000);
    producer_thread.join();
}

/** test with single consumer / single producer */
TEST(blocking_priority_queue, pop_tests)
{
    BlockingPriorityQueue<int64_t> queue(1010000);

    auto producer = [&]() {
        for (int64_t index = 0; index < 1'000'000; ++index) {
            queue.push(index);
        }
        queue.push(-1);
    };

    auto consumer = [&]() {
        auto result = queue.pop();
        int64_t count = 1;
        while (result >= 0) {
            auto next_result = queue.pop();
            if (next_result > result) {
                ++count;
            } else if (next_result > 0) {
                std::cout << next_result << " came before " << result << '\n';
            }
            result = next_result;
        }
        return count;
    };

    auto producer_thread = std::thread(producer);
    auto consumed_count = consumer();
    EXPECT_EQ(consumed_count, 1'000'000);
    producer_thread.join();
}

/** test with multiple consumer/single producer*/
TEST(blocking_priority_queue, multithreaded_tests2)
{
    BlockingPriorityQueue<int64_t> queue(1010000);

    for (int64_t index = 0; index < 10'000; ++index) {
        queue.push(index);
    }
    auto producer = [&]() {
        for (int64_t index = 10'000; index < 1'010'000; ++index) {
            queue.push(index);
        }
    };

    auto consumer = [&]() {
        auto result = queue.try_pop();
        int64_t count = 0;
        while (result) {
            ++count;
            result = queue.try_pop();
            if (!result) {  // make an additional sleep period so the producer
                            // can catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                result = queue.try_pop();
            }
        }
        return count;
    };

    std::packaged_task<int64_t()> first_task(consumer);
    std::packaged_task<int64_t()> second_task(consumer);
    auto first_result = first_task.get_future();
    auto second_result = second_task.get_future();
    auto first_consumer_thread = std::thread(std::move(first_task));
    auto second_consumer_thread = std::thread(std::move(second_task));
    auto producer_thread = std::thread(producer);
    auto third_count = consumer();
    auto first_count = first_result.get();
    auto second_count = second_result.get();

    EXPECT_EQ(first_count + second_count + third_count, 1'010'000);
    producer_thread.join();
    first_consumer_thread.join();
    second_consumer_thread.join();
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_priority_queue, multithreaded_tests3)
{
    BlockingPriorityQueue<int64_t> queue;
    queue.reserve(3'010'000);
    for (int64_t index = 0; index < 10'000; ++index) {
        queue.push(index);
    }
    auto producer = [&]() {
        for (int64_t index = 0; index < 1'000'000; ++index) {
            queue.push(index);
        }
    };

    auto consumer = [&]() {
        auto result = queue.try_pop();
        int64_t count = 0;
        while (result) {
            ++count;
            result = queue.try_pop();
            if (!result) {  // make an additional sleep period so the producer
                            // can catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                result = queue.try_pop();
            }
        }
        return count;
    };

    std::packaged_task<int64_t()> first_task(consumer);
    std::packaged_task<int64_t()> second_task(consumer);
    auto first_result = first_task.get_future();
    auto second_result = second_task.get_future();
    auto first_consumer_thread = std::thread(std::move(first_task));
    auto second_consumer_thread = std::thread(std::move(second_task));
    auto first_producer_thread = std::thread(producer);
    auto second_producer_thread = std::thread(producer);
    auto third_producer_thread = std::thread(producer);

    auto third_count = consumer();
    auto first_count = first_result.get();
    auto second_count = second_result.get();

    EXPECT_EQ(first_count + second_count + third_count, 3'010'000);

    first_producer_thread.join();
    second_producer_thread.join();
    third_producer_thread.join();
    first_consumer_thread.join();
    second_consumer_thread.join();
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_priority_queue, multithreaded_tests3_pop)
{
    BlockingPriorityQueue<int64_t> queue;
    queue.reserve(3'010'000);

    auto producer = [&]() {
        for (int64_t index = 0; index < 1'000'000; ++index) {
            queue.push(index);
        }
        queue.push(-1);
    };

    auto consumer = [&]() {
        int64_t result{1};
        int64_t count{0};
        while (result >= 0) {
            ++count;
            result = queue.pop();
        }
        return count;
    };

    std::packaged_task<int64_t()> first_task(consumer);
    std::packaged_task<int64_t()> second_task(consumer);
    auto first_result = first_task.get_future();
    auto second_result = second_task.get_future();
    auto first_consumer_thread = std::thread(std::move(first_task));
    auto second_consumer_thread = std::thread(std::move(second_task));
    auto first_producer_thread = std::thread(producer);
    auto second_producer_thread = std::thread(producer);
    auto third_producer_thread = std::thread(producer);
    auto third_count = consumer();

    first_producer_thread.join();
    second_producer_thread.join();
    third_producer_thread.join();

    auto first_count = first_result.get();
    auto second_count = second_result.get();

    EXPECT_EQ(first_count + second_count + third_count, 3'000'003);

    std::cout << "production complete\n";
    first_consumer_thread.join();
    second_consumer_thread.join();
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_priority_queue, pop_callback_tests)
{
    BlockingPriorityQueue<int64_t> queue;
    int push_count = 0;
    auto producer = [&]() {
        queue.push(7);
        ++push_count;
    };

    auto consumer = [&](int count) {
        for (int index = 0; index < count; ++index) {
            queue.popOrCall(producer);
        }
        return count;
    };

    auto first_result = consumer(25);
    EXPECT_EQ(first_result, 25);
    EXPECT_EQ(push_count, 25);
    auto second_result = consumer(127);
    EXPECT_EQ(second_result, 127);
    EXPECT_EQ(push_count, 127 + 25);
}
