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
#include <mutex>
#include <thread>
#include <utility>
/** these test cases test data_block and data_view objects
 */

#include "BlockingQueue.hpp"
using gmlc::containers::BlockingQueue;

/** test basic operations */
TEST(blocking_queue, basic)
{
    BlockingQueue<int> queue;

    queue.push(45);
    queue.push(54);

    EXPECT_FALSE(queue.empty());

    EXPECT_EQ(queue.size(), 2);
    auto popped_value = queue.try_pop();
    EXPECT_EQ(*popped_value, 45);
    popped_value = queue.try_pop();
    EXPECT_EQ(*popped_value, 54);

    popped_value = queue.try_pop();
    EXPECT_FALSE(popped_value);
    EXPECT_TRUE(queue.empty());
}

/** test with a move only element*/
TEST(blocking_queue, move_only)
{
    BlockingQueue<std::unique_ptr<double>> queue;

    queue.push(std::make_unique<double>(4534.23));

    auto second_element = std::make_unique<double>(34.234);
    queue.push(std::move(second_element));

    EXPECT_FALSE(queue.empty());

    EXPECT_EQ(queue.size(), 2);
    auto popped_value = queue.try_pop();
    EXPECT_EQ(**popped_value, 4534.23);
    popped_value = queue.try_pop();
    EXPECT_EQ(**popped_value, 34.234);

    popped_value = queue.try_pop();
    EXPECT_FALSE(popped_value);
    EXPECT_TRUE(queue.empty());
}

/** test the ordering with a larger number of inputs*/
TEST(blocking_queue, ordering)
{
    BlockingQueue<int> queue;

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
    for (int index = 7; index < 20; ++index) {
        popped_value = queue.try_pop();
        EXPECT_EQ(*popped_value, index);
    }

    EXPECT_TRUE(queue.empty());
}

TEST(blocking_queue, emplace)
{
    BlockingQueue<std::pair<int, double>> queue;

    queue.emplace(10, 45.4);
    queue.emplace(11, 34.1);
    queue.emplace(12, 34.2);

    EXPECT_EQ(queue.size(), 3);
    auto popped_value = queue.try_pop();
    EXPECT_EQ(popped_value->first, 10);
    EXPECT_EQ(popped_value->second, 45.4);
    popped_value = queue.try_pop();
    EXPECT_EQ(popped_value->first, 11);
    EXPECT_EQ(popped_value->second, 34.1);
}

/** test with single consumer/single producer*/
TEST(blocking_queue, multithreaded)
{
    BlockingQueue<int64_t> queue(1010000);

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

    auto producer_task = std::async(std::launch::async, producer);
    auto consumer_task = std::async(std::launch::async, consumer);

    producer_task.wait();
    auto consumed_count = consumer_task.get();
    EXPECT_EQ(consumed_count, 1'010'000);
}

/** test with single consumer / single producer */
TEST(blocking_queue, pop)
{
    BlockingQueue<int64_t> queue(1010000);

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

    auto producer_task = std::async(std::launch::async, producer);
    auto consumer_task = std::async(std::launch::async, consumer);

    producer_task.wait();
    auto consumed_count = consumer_task.get();
    EXPECT_EQ(consumed_count, 1'000'000);
}

/** test with multiple consumer/single producer*/
TEST(blocking_queue, multithreaded2)
{
    BlockingQueue<int64_t> queue(1010000);

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

    auto producer_task = std::async(std::launch::async, producer);

    auto first_consumer_task = std::async(std::launch::async, consumer);
    auto second_consumer_task = std::async(std::launch::async, consumer);
    auto third_consumer_task = std::async(std::launch::async, consumer);
    producer_task.wait();
    auto first_count = first_consumer_task.get();
    auto second_count = second_consumer_task.get();
    auto third_count = third_consumer_task.get();

    EXPECT_EQ(first_count + second_count + third_count, 1'010'000);
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_queue, multithreaded3)
{
    std::mutex printer;
    BlockingQueue<int64_t> queue;
    queue.reserve(3'010'000);
    for (int64_t index = 0; index < 10'000; ++index) {
        queue.push(index);
    }
    auto producer = [&]() {
        for (int64_t index = 0; index < 1'000'000; ++index) {
            queue.push(index);
        }
        const std::lock_guard<std::mutex> print_lock(printer);
        std::cout << "produce completed\n";
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
                const std::lock_guard<std::mutex> print_lock(printer);
                std::cout << "sleeping " << count << '\n';
                result = queue.try_pop();
            }
        }
        return count;
    };

    auto first_producer_task = std::async(std::launch::async, producer);
    auto second_producer_task = std::async(std::launch::async, producer);
    auto third_producer_task = std::async(std::launch::async, producer);

    auto first_consumer_task = std::async(std::launch::async, consumer);
    auto second_consumer_task = std::async(std::launch::async, consumer);
    auto third_consumer_task = std::async(std::launch::async, consumer);
    std::unique_lock<std::mutex> print_lock(printer);
    std::cout << "launched\n";
    print_lock.unlock();
    first_producer_task.wait();
    second_producer_task.wait();
    third_producer_task.wait();
    print_lock.lock();
    std::cout << "production complete\n";
    print_lock.unlock();
    auto first_count = first_consumer_task.get();
    auto second_count = second_consumer_task.get();
    print_lock.lock();
    std::cout << "got2\n";
    print_lock.unlock();
    auto third_count = third_consumer_task.get();

    EXPECT_EQ(first_count + second_count + third_count, 3'010'000);
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_queue, multithreaded_tests3_pop)
{
    BlockingQueue<int64_t> queue;
    queue.reserve(3'010'000);

    auto producer = [&]() {
        for (int64_t index = 0; index < 1'000'000; ++index) {
            queue.push(index);
        }
        queue.push(-1);
    };

    auto consumer = [&]() {
        auto result = queue.pop();
        int64_t count = 0;
        while (result >= 0) {
            ++count;
            result = queue.pop();
        }
        return count;
    };

    auto first_producer_task = std::async(std::launch::async, producer);
    auto second_producer_task = std::async(std::launch::async, producer);
    auto third_producer_task = std::async(std::launch::async, producer);

    auto first_consumer_task = std::async(std::launch::async, consumer);
    auto second_consumer_task = std::async(std::launch::async, consumer);
    auto third_consumer_task = std::async(std::launch::async, consumer);
    first_producer_task.wait();
    second_producer_task.wait();
    third_producer_task.wait();
    auto first_count = first_consumer_task.get();
    auto second_count = second_consumer_task.get();
    auto third_count = third_consumer_task.get();

    EXPECT_EQ(first_count + second_count + third_count, 3'000'000);
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_queue, pop_callback_tests)
{
    BlockingQueue<int64_t> queue;
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
