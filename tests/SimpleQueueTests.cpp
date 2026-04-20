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
#include <thread>
#include <utility>
#include <vector>
/** these test cases test data_block and data_view objects
 */

#include "SimpleQueue.hpp"
using gmlc::containers::SimpleQueue;

/** test basic operations */
TEST(simple_queue_tests, basic_tests)
{
    SimpleQueue<int> queue;

    queue.push(45);
    queue.push(54);

    EXPECT_FALSE(queue.empty());

    EXPECT_EQ(queue.size(), 2);
    auto popped_value = queue.pop();
    EXPECT_EQ(*popped_value, 45);
    popped_value = queue.pop();
    EXPECT_EQ(*popped_value, 54);

    popped_value = queue.pop();
    EXPECT_FALSE(popped_value);
    EXPECT_TRUE(queue.empty());
}

/** test with a move only element*/
TEST(simple_queue_tests, move_only_tests)
{
    SimpleQueue<std::unique_ptr<double>> queue;

    queue.push(std::make_unique<double>(4534.23));

    auto second_element = std::make_unique<double>(34.234);
    queue.push(std::move(second_element));

    EXPECT_FALSE(queue.empty());

    EXPECT_EQ(queue.size(), 2);
    auto popped_value = queue.pop();
    EXPECT_EQ(**popped_value, 4534.23);
    popped_value = queue.pop();
    EXPECT_EQ(**popped_value, 34.234);

    popped_value = queue.pop();
    EXPECT_FALSE(popped_value);
    EXPECT_TRUE(queue.empty());
}

/** test the ordering with a larger number of inputs*/
TEST(simple_queue_tests, ordering_tests)
{
    SimpleQueue<int> queue;

    for (int index = 1; index < 10; ++index) {
        queue.push(index);
    }

    auto popped_value = queue.pop();
    EXPECT_EQ(*popped_value, 1);
    for (int index = 2; index < 7; ++index) {
        popped_value = queue.pop();
        EXPECT_EQ(*popped_value, index);
    }
    for (int index = 10; index < 20; ++index) {
        queue.push(index);
    }
    for (int index = 7; index < 20; ++index) {
        popped_value = queue.pop();
        EXPECT_EQ(*popped_value, index);
    }

    EXPECT_TRUE(queue.empty());
}

TEST(simple_queue_tests, emplace_tests)
{
    SimpleQueue<std::pair<int, double>> queue;

    queue.emplace(10, 45.4);
    queue.emplace(11, 34.1);
    queue.emplace(12, 34.2);

    EXPECT_EQ(queue.size(), 3);
    auto popped_value = queue.pop();
    EXPECT_EQ(popped_value->first, 10);
    EXPECT_EQ(popped_value->second, 45.4);
    popped_value = queue.pop();
    EXPECT_EQ(popped_value->first, 11);
    EXPECT_EQ(popped_value->second, 34.1);
}

/** test with single consumer/single producer*/
TEST(simple_queue_tests, multithreaded_tests)
{
    SimpleQueue<int64_t> queue(1010000);

    for (int64_t index = 0; index < 10'000; ++index) {
        queue.push(index);
    }
    auto producer = [&]() {
        for (int64_t index = 10'000; index < 1'010'000; ++index) {
            queue.push(index);
        }
    };

    auto consumer = [&]() {
        auto result = queue.pop();
        int64_t count = 1;
        while (result) {
            auto next_result = queue.pop();
            if (!next_result) {  // make an additional sleep period so the producer can
                                 // catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                next_result = queue.pop();
            }
            if (next_result) {
                if (*next_result > *result) {
                    ++count;
                } else if (*next_result > 0) {
                    std::cout << *next_result << " came before " << *result
                              << '\n';
                }
            }
            result = next_result;
        }
        return count;
    };

    auto producer_task = std::async(std::launch::async, producer);
    auto consumer_task = std::async(std::launch::async, consumer);

    producer_task.wait();
    auto consumed_count = consumer_task.get();
    EXPECT_EQ(consumed_count, 1'010'000);
}

/** test with multiple consumer/single producer*/
TEST(simple_queue_tests, multithreaded_tests2)
{
    SimpleQueue<int64_t> queue(1010000);

    for (int64_t index = 0; index < 10'000; ++index) {
        queue.push(index);
    }
    auto producer = [&]() {
        for (int64_t index = 10'000; index < 2'010'000; ++index) {
            queue.push(index);
        }
    };

    auto consumer = [&]() {
        auto result = queue.pop();
        int64_t count = 0;
        while (result) {
            ++count;
            result = queue.pop();
            if (!result) {  // make an additional sleep period so the producer can
                            // catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                result = queue.pop();
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

    EXPECT_EQ(first_count + second_count + third_count, 2'010'000);
}

/** test with multiple producer/multiple consumer*/
TEST(simple_queue_tests, multithreaded_tests3)
{
    SimpleQueue<int64_t> queue;
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
        auto result = queue.pop();
        int64_t count = 0;
        while (result) {
            ++count;
            result = queue.pop();
            if (!result) {  // make an additional sleep period so the producer can
                            // catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                result = queue.pop();
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
    first_producer_task.wait();
    second_producer_task.wait();
    third_producer_task.wait();
    auto first_count = first_consumer_task.get();
    auto second_count = second_consumer_task.get();
    auto third_count = third_consumer_task.get();

    EXPECT_EQ(first_count + second_count + third_count, 3'010'000);
}

/** test with multiple producer/multiple consumer*/
TEST(simple_queue_tests, move_construct)
{
    SimpleQueue<int64_t> queue;
    queue.push(54);
    queue.push(55);
    SimpleQueue<int64_t> moved_queue(std::move(queue));

    auto result = moved_queue.pop();
    EXPECT_EQ(*result, 54);
    result = moved_queue.pop();
    EXPECT_EQ(*result, 55);
    result = moved_queue.pop();
    EXPECT_FALSE(result);
}

/** test with multiple producer/multiple consumer*/
TEST(simple_queue_tests, move_assign)
{
    SimpleQueue<int64_t> queue;
    queue.push(54);
    queue.push(55);
    SimpleQueue<int64_t> target_queue;
    target_queue = std::move(queue);

    auto peeked_result = target_queue.peek();
    EXPECT_EQ(*peeked_result, 54);
    auto result = target_queue.pop();
    EXPECT_EQ(*result, 54);

    result = target_queue.pop();
    EXPECT_EQ(*result, 55);
    result = target_queue.pop();
    EXPECT_FALSE(result);
    peeked_result = target_queue.peek();
    EXPECT_FALSE(peeked_result);
}

/** test with multiple producer/multiple consumer*/
TEST(simple_queue_tests, pop_test)
{
    SimpleQueue<int64_t> queue;
    queue.push(54);
    queue.push(55);
    queue.push(56);
    queue.pushVector(std::vector<int64_t>{57, 58, 59});
    queue.pop();
    queue.pop();
    queue.push(60);
    EXPECT_TRUE(queue.pop());
    EXPECT_TRUE(queue.pop());
    EXPECT_TRUE(queue.pop());
    EXPECT_TRUE(queue.pop());
    EXPECT_TRUE(queue.pop());
}
