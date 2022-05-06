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
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

// Modified for gmlc/containers 7/23/2019

// test case for WorkQueue

#include "WorkQueue.hpp"

#include "gtest/gtest.h"
#include <chrono>
#include <iostream>

using namespace gmlc::containers;

TEST(work_queue, WorkQueue_test1)
{
    WorkQueue wq(1);

    EXPECT_EQ(wq.getWorkerCount(), 1);
    std::cout << "loc1" << std::endl;
    wq.closeWorkerQueue();
    std::cout << "loc2" << std::endl;
    EXPECT_EQ(wq.getWorkerCount(), 0);
    std::cout << "loc3" << std::endl;
    auto fk = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return std::hash<std::thread::id>()(std::this_thread::get_id());
    };

    WorkQueue wq2(5);
    std::cout << "loc4" << std::endl;
    EXPECT_EQ(wq2.getWorkerCount(), 5);
    std::vector<decltype(make_shared_workBlock(fk))> blocks(10);
    std::vector<std::shared_ptr<basicWorkBlock>> bblocks(10);
    for (size_t kk = 0; kk < 10; ++kk) {
        blocks[kk] = make_shared_workBlock(fk);
        bblocks[kk] = blocks[kk];
    }
    std::cout << "loc5" << std::endl;
    wq2.addWorkBlock(bblocks);
    std::cout << "loc6" << std::endl;
    std::vector<decltype(fk())> res(10);
    for (size_t kk = 0; kk < 10; ++kk) {
        res[kk] = blocks[kk]->getReturnVal();
    }
    std::cout << "loc7" << std::endl;
    std::sort(res.begin(), res.end());
    auto last = std::unique(res.begin(), res.end());
    res.erase(last, res.end());
    EXPECT_EQ(res.size(), 5u);
    std::cout << "loc8" << std::endl;
}

TEST(work_queue, WorkQueue_test2)
{
    // Test a zero worker count
    WorkQueue wq(0);
    std::function<void()> fk = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(110));
    };

    auto b1 = make_shared_workBlock(fk);
    auto start_t = std::chrono::high_resolution_clock::now();
    wq.addWorkBlock(b1);
    auto stop_t = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = stop_t - start_t;
    // verifying the block was run immediately
    EXPECT_GE(elapsed_time.count(), 0.1);
    // verify the block won't run again until reset
    start_t = std::chrono::high_resolution_clock::now();
    wq.addWorkBlock(b1);
    stop_t = std::chrono::high_resolution_clock::now();
    elapsed_time = stop_t - start_t;
    // verifying the block was run immediately
    EXPECT_LT(elapsed_time.count(), 0.05);
    // verify function update works properly
    fk = [] { std::this_thread::sleep_for(std::chrono::milliseconds(130)); };
    b1->updateWorkFunction(fk);

    start_t = std::chrono::high_resolution_clock::now();
    wq.addWorkBlock(b1);
    stop_t = std::chrono::high_resolution_clock::now();
    elapsed_time = stop_t - start_t;
    EXPECT_GE(elapsed_time.count(), 0.125);

    wq.closeWorkerQueue();

    WorkQueue wq2(1);
    b1->reset();
    start_t = std::chrono::high_resolution_clock::now();
    wq2.addWorkBlock(b1);
    stop_t = std::chrono::high_resolution_clock::now();
    elapsed_time = stop_t - start_t;
    // verifying the block was run immediately
    EXPECT_LT(elapsed_time.count(), 0.05);
    wq.closeWorkerQueue();
}

TEST(work_queue, WorkQueue_test3)
{
    // Test a queue priority mechanisms

    WorkQueue wq(1);
    // a sleeper work block to give us time to set up the rest
    auto fk = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    };

    auto b1 = make_workBlock(fk);
    // only 1 worker thread so don't worry about locking
    std::vector<int> order;

    auto hp = [&order] { order.push_back(1); };
    auto mp = [&order] { order.push_back(2); };
    auto lp = [&order] { order.push_back(3); };
    wq.setPriorityRatio(3);
    wq.addWorkBlock(std::move(b1), WorkQueue::workPriority::high);

    wq.addWorkBlock(make_workBlock(lp), WorkQueue::workPriority::low);
    wq.addWorkBlock(make_workBlock(lp), WorkQueue::workPriority::low);
    wq.addWorkBlock(make_workBlock(lp), WorkQueue::workPriority::low);

    wq.addWorkBlock(make_workBlock(mp), WorkQueue::workPriority::medium);
    wq.addWorkBlock(make_workBlock(mp), WorkQueue::workPriority::medium);
    wq.addWorkBlock(make_workBlock(mp), WorkQueue::workPriority::medium);
    wq.addWorkBlock(make_workBlock(mp), WorkQueue::workPriority::medium);
    wq.addWorkBlock(make_workBlock(mp), WorkQueue::workPriority::medium);
    wq.addWorkBlock(make_workBlock(mp), WorkQueue::workPriority::medium);
    wq.addWorkBlock(make_workBlock(mp), WorkQueue::workPriority::medium);
    wq.addWorkBlock(make_workBlock(mp), WorkQueue::workPriority::medium);
    wq.addWorkBlock(make_workBlock(mp), WorkQueue::workPriority::medium);

    wq.addWorkBlock(make_workBlock(hp), WorkQueue::workPriority::high);
    wq.addWorkBlock(make_workBlock(hp), WorkQueue::workPriority::high);
    std::this_thread::sleep_for(std::chrono::milliseconds(340));
    while (!wq.isEmpty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    EXPECT_EQ(order.size(), 14u);
    std::vector<int> orderCorrect = {1, 1, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3};
    int cdiff = 0;
    for (size_t kk = 0; kk < 14; ++kk) {
        if (order[kk] != orderCorrect[kk]) {
            ++cdiff;
        }
    }
    EXPECT_EQ(cdiff, 0) << "Execution out of order";
}

TEST(work_queue, WorkQueue_test3_vector)
{
    // Test a queue priority mechanisms

    WorkQueue wq(1);
    // a sleeper work block to give us time to set up the rest
    auto fk = [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    };

    auto b1 = make_workBlock(fk);
    // only 1 worker thread so don't worry about locking
    std::vector<int> order;

    auto hp = [&order] { order.push_back(1); };
    auto mp = [&order] { order.push_back(2); };
    auto lp = [&order] { order.push_back(3); };
    std::vector<std::shared_ptr<basicWorkBlock>> lpv;
    for (int ii = 0; ii < 3; ++ii) {
        auto res = make_workBlock(lp);
        lpv.push_back(std::move(res));
    }

    std::vector<std::shared_ptr<basicWorkBlock>> mpv;
    for (int ii = 0; ii < 9; ++ii) {
        auto res = make_workBlock(mp);
        mpv.push_back(std::move(res));
    }

    std::vector<std::shared_ptr<basicWorkBlock>> hpv;
    for (int ii = 0; ii < 2; ++ii) {
        auto res = make_workBlock(hp);
        hpv.push_back(std::move(res));
    }

    wq.setPriorityRatio(3);
    wq.addWorkBlock(std::move(b1), WorkQueue::workPriority::high);

    wq.addWorkBlock(lpv, WorkQueue::workPriority::low);
    wq.addWorkBlock(mpv, WorkQueue::workPriority::medium);
    wq.addWorkBlock(hpv, WorkQueue::workPriority::high);

    std::this_thread::sleep_for(std::chrono::milliseconds(340));
    while (!wq.isEmpty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    EXPECT_EQ(order.size(), 14u);
    std::vector<int> orderCorrect = {1, 1, 2, 2, 2, 3, 2, 2, 2, 3, 2, 2, 2, 3};
    int cdiff = 0;
    for (size_t kk = 0; kk < 14; ++kk) {
        if (order[kk] != orderCorrect[kk]) {
            ++cdiff;
        }
    }
    EXPECT_EQ(cdiff, 0) << "Execution out of order";
}
