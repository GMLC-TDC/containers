/*
 * LLNS Copyright Start
 * Copyright (c) 2014-2018, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 */

/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

// Modified for gmlc/containers 7/23/2019

#include "WorkQueue.hpp"

#include "gtest/gtest.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

using gmlc::containers::BasicWorkBlock;
using gmlc::containers::make_shared_workBlock;
using gmlc::containers::make_workBlock;
using gmlc::containers::WorkQueue;

TEST(work_queue, WorkQueue_test1)
{
    WorkQueue work_queue(1);

    EXPECT_EQ(work_queue.getWorkerCount(), 1);
    work_queue.closeWorkerQueue();
    EXPECT_EQ(work_queue.getWorkerCount(), 0);
    auto work_function = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return std::hash<std::thread::id>()(std::this_thread::get_id());
    };

    WorkQueue work_queue2(4);
    constexpr std::size_t block_count{20};
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_EQ(work_queue2.getWorkerCount(), 4);
    std::vector<decltype(make_shared_workBlock(work_function))> blocks(
        block_count);
    std::vector<std::shared_ptr<BasicWorkBlock>> basic_blocks(block_count);
    for (std::size_t block_index = 0; block_index < block_count;
         ++block_index) {
        blocks[block_index] = make_shared_workBlock(work_function);
        basic_blocks[block_index] = blocks[block_index];
    }
    work_queue2.addWorkBlock(basic_blocks);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::vector<decltype(work_function())> results(block_count);
    for (std::size_t block_index = 0; block_index < block_count;
         ++block_index) {
        results[block_index] = blocks[block_index]->getReturnVal();
    }
    std::sort(results.begin(), results.end());
    const auto last = std::unique(results.begin(), results.end());
    results.erase(last, results.end());
    EXPECT_EQ(results.size(), 4U);
}

TEST(work_queue, WorkQueue_test2)
{
    WorkQueue work_queue(0);
    std::function<void()> work_function = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(110));
    };

    auto block1 = make_shared_workBlock(work_function);
    auto start_time = std::chrono::high_resolution_clock::now();
    work_queue.addWorkBlock(block1);
    auto stop_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = stop_time - start_time;
    EXPECT_GE(elapsed_time.count(), 0.1);

    start_time = std::chrono::high_resolution_clock::now();
    work_queue.addWorkBlock(block1);
    stop_time = std::chrono::high_resolution_clock::now();
    elapsed_time = stop_time - start_time;
    EXPECT_LT(elapsed_time.count(), 0.05);

    work_function = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(130));
    };
    block1->updateWorkFunction(work_function);

    start_time = std::chrono::high_resolution_clock::now();
    work_queue.addWorkBlock(block1);
    stop_time = std::chrono::high_resolution_clock::now();
    elapsed_time = stop_time - start_time;
    EXPECT_GE(elapsed_time.count(), 0.125);

    work_queue.closeWorkerQueue();

    WorkQueue work_queue2(1);
    block1->reset();
    start_time = std::chrono::high_resolution_clock::now();
    work_queue2.addWorkBlock(block1);
    stop_time = std::chrono::high_resolution_clock::now();
    elapsed_time = stop_time - start_time;
    EXPECT_LT(elapsed_time.count(), 0.05);
    work_queue.closeWorkerQueue();
}

TEST(work_queue, WorkQueue_test3)
{
    WorkQueue work_queue(1);
    auto sleeper = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    };

    auto block1 = make_workBlock(sleeper);
    std::vector<int> order;
    std::mutex lock;
    auto high_priority = [&order, &lock] {
        const std::unique_lock<std::mutex> queue_lock(lock);
        order.push_back(1);
    };
    auto medium_priority = [&order, &lock] {
        const std::unique_lock<std::mutex> queue_lock(lock);
        order.push_back(2);
    };
    auto low_priority = [&order, &lock] {
        const std::unique_lock<std::mutex> queue_lock(lock);
        order.push_back(3);
    };
    work_queue.setPriorityRatio(3);
    work_queue.addWorkBlock(std::move(block1), WorkQueue::WorkPriority::high);

    work_queue.addWorkBlock(
        make_workBlock(low_priority), WorkQueue::WorkPriority::low);
    work_queue.addWorkBlock(
        make_workBlock(low_priority), WorkQueue::WorkPriority::low);
    work_queue.addWorkBlock(
        make_workBlock(low_priority), WorkQueue::WorkPriority::low);

    work_queue.addWorkBlock(
        make_workBlock(medium_priority), WorkQueue::WorkPriority::medium);
    work_queue.addWorkBlock(
        make_workBlock(medium_priority), WorkQueue::WorkPriority::medium);
    work_queue.addWorkBlock(
        make_workBlock(medium_priority), WorkQueue::WorkPriority::medium);
    work_queue.addWorkBlock(
        make_workBlock(medium_priority), WorkQueue::WorkPriority::medium);
    work_queue.addWorkBlock(
        make_workBlock(medium_priority), WorkQueue::WorkPriority::medium);
    work_queue.addWorkBlock(
        make_workBlock(medium_priority), WorkQueue::WorkPriority::medium);
    work_queue.addWorkBlock(
        make_workBlock(medium_priority), WorkQueue::WorkPriority::medium);
    work_queue.addWorkBlock(
        make_workBlock(medium_priority), WorkQueue::WorkPriority::medium);
    work_queue.addWorkBlock(
        make_workBlock(medium_priority), WorkQueue::WorkPriority::medium);

    work_queue.addWorkBlock(
        make_workBlock(high_priority), WorkQueue::WorkPriority::high);
    work_queue.addWorkBlock(
        make_workBlock(high_priority), WorkQueue::WorkPriority::high);
    std::this_thread::sleep_for(std::chrono::milliseconds(350));
    while (!work_queue.isEmpty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::unique_lock<std::mutex> queue_lock(lock);
    if (order.size() < 14U) {
        queue_lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(350));
        while (!work_queue.isEmpty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        queue_lock.lock();
    }
    EXPECT_EQ(order.size(), 14U);

    std::vector<int> correct_order = {1, 1, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3};
    int difference_count = 0;
    for (std::size_t block_index = 0; block_index < order.size();
         ++block_index) {
        if (order[block_index] != correct_order[block_index]) {
            ++difference_count;
        }
    }
    EXPECT_EQ(difference_count, 0) << "Execution out of order";
}

TEST(work_queue, WorkQueue_test3_vector)
{
    WorkQueue work_queue(1);
    work_queue.setPriorityRatio(3);
    auto sleeper = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    };

    auto block1 = make_workBlock(sleeper);
    std::vector<int> order;
    std::mutex lock;
    auto high_priority = [&order, &lock] {
        const std::lock_guard<std::mutex> queue_lock(lock);
        order.push_back(1);
    };
    auto medium_priority = [&order, &lock] {
        const std::lock_guard<std::mutex> queue_lock(lock);
        order.push_back(2);
    };
    auto low_priority = [&order, &lock] {
        const std::lock_guard<std::mutex> queue_lock(lock);
        order.push_back(3);
    };
    std::vector<std::shared_ptr<BasicWorkBlock>> low_priority_vector;
    for (int block_index = 0; block_index < 3; ++block_index) {
        auto result = make_workBlock(low_priority);
        low_priority_vector.push_back(std::move(result));
    }

    std::vector<std::shared_ptr<BasicWorkBlock>> medium_priority_vector;
    for (int block_index = 0; block_index < 9; ++block_index) {
        auto result = make_workBlock(medium_priority);
        medium_priority_vector.push_back(std::move(result));
    }

    std::vector<std::shared_ptr<BasicWorkBlock>> high_priority_vector;
    for (int block_index = 0; block_index < 2; ++block_index) {
        auto result = make_workBlock(high_priority);
        high_priority_vector.push_back(std::move(result));
    }

    work_queue.addWorkBlock(std::move(block1), WorkQueue::WorkPriority::high);

    work_queue.addWorkBlock(low_priority_vector, WorkQueue::WorkPriority::low);
    work_queue.addWorkBlock(
        medium_priority_vector, WorkQueue::WorkPriority::medium);
    work_queue.addWorkBlock(
        high_priority_vector, WorkQueue::WorkPriority::high);

    std::this_thread::sleep_for(std::chrono::milliseconds(350));

    while (!work_queue.isEmpty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::unique_lock<std::mutex> queue_lock(lock);
    if (order.size() < 14U) {
        queue_lock.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(350));
        while (!work_queue.isEmpty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        queue_lock.lock();
    }
    std::vector<int> correct_order = {1, 1, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3};
    int difference_count = 0;
    for (std::size_t block_index = 0; block_index < 14U; ++block_index) {
        if (order[block_index] != correct_order[block_index]) {
            ++difference_count;
        }
    }
    EXPECT_EQ(difference_count, 0) << "Execution out of order";
}
