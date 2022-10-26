/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "gtest/gtest.h"
#include <future>
#include <iostream>
#include <memory>
#include <thread>
/** these test cases test data_block and data_view objects
 */

#include "BlockingQueue.hpp"
using namespace gmlc::containers;

/** test basic operations */
TEST(blocking_queue, basic)
{
    BlockingQueue<int> sq;

    sq.push(45);
    sq.push(54);

    EXPECT_TRUE(sq.empty() == false);

    EXPECT_EQ(sq.size(), 2);
    auto b = sq.try_pop();
    EXPECT_EQ(*b, 45);
    b = sq.try_pop();
    EXPECT_EQ(*b, 54);

    b = sq.try_pop();
    EXPECT_TRUE(!(b));
    EXPECT_TRUE(sq.empty());
}

/** test with a move only element*/
TEST(blocking_queue, move_only)
{
    BlockingQueue<std::unique_ptr<double>> sq;

    sq.push(std::make_unique<double>(4534.23));

    auto e2 = std::make_unique<double>(34.234);
    sq.push(std::move(e2));

    EXPECT_TRUE(sq.empty() == false);

    EXPECT_EQ(sq.size(), 2);
    auto b = sq.try_pop();
    EXPECT_EQ(**b, 4534.23);
    b = sq.try_pop();
    EXPECT_EQ(**b, 34.234);

    b = sq.try_pop();
    EXPECT_TRUE(!(b));
    EXPECT_TRUE(sq.empty());
}

/** test the ordering with a larger number of inputs*/

TEST(blocking_queue, ordering)
{
    BlockingQueue<int> sq;

    for (int ii = 1; ii < 10; ++ii) {
        sq.push(ii);
    }

    auto b = sq.try_pop();
    EXPECT_EQ(*b, 1);
    for (int ii = 2; ii < 7; ++ii) {
        b = sq.try_pop();
        EXPECT_EQ(*b, ii);
    }
    for (int ii = 10; ii < 20; ++ii) {
        sq.push(ii);
    }
    for (int ii = 7; ii < 20; ++ii) {
        b = sq.try_pop();
        EXPECT_EQ(*b, ii);
    }

    EXPECT_TRUE(sq.empty());
}

TEST(blocking_queue, emplace)
{
    BlockingQueue<std::pair<int, double>> sq;

    sq.emplace(10, 45.4);
    sq.emplace(11, 34.1);
    sq.emplace(12, 34.2);

    EXPECT_EQ(sq.size(), 3);
    auto b = sq.try_pop();
    EXPECT_EQ(b->first, 10);
    EXPECT_EQ(b->second, 45.4);
    b = sq.try_pop();
    EXPECT_EQ(b->first, 11);
    EXPECT_EQ(b->second, 34.1);
}

/** test with single consumer/single producer*/
TEST(blocking_queue, multithreaded)
{
    BlockingQueue<int64_t> sq(1010000);

    for (int64_t ii = 0; ii < 10'000; ++ii) {
        sq.push(ii);
    }
    auto prod1 = [&]() {
        for (int64_t ii = 10'000; ii < 1'010'000; ++ii) {
            sq.push(ii);
        }
    };

    auto cons = [&]() {
        auto res = sq.try_pop();
        int64_t cnt = 0;
        while ((res)) {
            ++cnt;
            res = sq.try_pop();
            if (!res) {  // make an additional sleep period so the producer can
                         // catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                res = sq.try_pop();
            }
        }
        return cnt;
    };

    auto ret = std::async(std::launch::async, prod1);

    auto res = std::async(std::launch::async, cons);

    ret.wait();
    auto V = res.get();
    EXPECT_EQ(V, 1'010'000);
}

/** test with single consumer / single producer */
TEST(blocking_queue, pop)
{
    BlockingQueue<int64_t> sq(1010000);

    auto prod1 = [&]() {
        for (int64_t ii = 0; ii < 1'000'000; ++ii) {
            sq.push(ii);
        }
        sq.push(-1);
    };

    auto cons = [&]() {
        auto res = sq.pop();
        int64_t cnt = 1;
        while (res >= 0) {
            auto nres = sq.pop();
            if (nres > res) {
                ++cnt;
            } else {
                if (nres > 0) {
                    printf(
                        "%d came before %d\n",
                        static_cast<int>(nres),
                        static_cast<int>(res));
                }
            }
            res = nres;
        }
        return cnt;
    };

    auto ret = std::async(std::launch::async, prod1);

    auto res = std::async(std::launch::async, cons);

    ret.wait();
    auto V = res.get();
    EXPECT_EQ(V, 1'000'000);
}

/** test with multiple consumer/single producer*/
TEST(blocking_queue, multithreaded2)
{
    BlockingQueue<int64_t> sq(1010000);

    for (int64_t ii = 0; ii < 10'000; ++ii) {
        sq.push(ii);
    }
    auto prod1 = [&]() {
        for (int64_t ii = 10'000; ii < 1'010'000; ++ii) {
            sq.push(ii);
        }
    };

    auto cons = [&]() {
        auto res = sq.try_pop();
        int64_t cnt = 0;
        while ((res)) {
            ++cnt;
            res = sq.try_pop();
            if (!res) {  // make an additional sleep period so the producer can
                         // catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                res = sq.try_pop();
            }
        }
        return cnt;
    };

    auto ret = std::async(std::launch::async, prod1);

    auto res1 = std::async(std::launch::async, cons);
    auto res2 = std::async(std::launch::async, cons);
    auto res3 = std::async(std::launch::async, cons);
    ret.wait();
    auto V1 = res1.get();
    auto V2 = res2.get();
    auto V3 = res3.get();

    EXPECT_EQ(V1 + V2 + V3, 1'010'000);
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_queue, multithreaded3)
{
    std::mutex printer;
    BlockingQueue<int64_t> sq;
    sq.reserve(3'010'000);
    for (int64_t ii = 0; ii < 10'000; ++ii) {
        sq.push(ii);
    }
    auto prod1 = [&]() {
        for (int64_t ii = 0; ii < 1'000'000; ++ii) {
            sq.push(ii);
        }
        std::lock_guard<std::mutex> printlock(printer);
        std::cout << "produce completed" << std::endl;
    };

    auto cons = [&]() {
        auto res = sq.try_pop();
        int64_t cnt = 0;
        while ((res)) {
            ++cnt;
            res = sq.try_pop();
            if (!res) {  // make an additional sleep period so the producer can
                         // catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::lock_guard<std::mutex> printlock(printer);
                std::cout << "sleeping " << cnt << std::endl;
                res = sq.try_pop();
            }
        }
        return cnt;
    };

    auto ret1 = std::async(std::launch::async, prod1);
    auto ret2 = std::async(std::launch::async, prod1);
    auto ret3 = std::async(std::launch::async, prod1);

    auto res1 = std::async(std::launch::async, cons);
    auto res2 = std::async(std::launch::async, cons);
    auto res3 = std::async(std::launch::async, cons);
    std::unique_lock<std::mutex> printlock(printer);
    std::cout << "launched" << std::endl;
    printlock.unlock();
    ret1.wait();
    ret2.wait();
    ret3.wait();
    printlock.lock();
    std::cout << "production complete" << std::endl;
    printlock.unlock();
    auto V1 = res1.get();
    auto V2 = res2.get();
    printlock.lock();
    std::cout << "got2" << std::endl;
    printlock.unlock();
    auto V3 = res3.get();

    EXPECT_EQ(V1 + V2 + V3, 3'010'000);
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_queue, multithreaded_tests3_pop)
{
    BlockingQueue<int64_t> sq;
    sq.reserve(3'010'000);

    auto prod1 = [&]() {
        for (int64_t ii = 0; ii < 1'000'000; ++ii) {
            sq.push(ii);
        }
        sq.push(-1);
    };

    auto cons = [&]() {
        auto res = sq.pop();
        int64_t cnt = 0;
        while (res >= 0) {
            ++cnt;
            res = sq.pop();
        }
        return cnt;
    };

    auto ret1 = std::async(std::launch::async, prod1);
    auto ret2 = std::async(std::launch::async, prod1);
    auto ret3 = std::async(std::launch::async, prod1);

    auto res1 = std::async(std::launch::async, cons);
    auto res2 = std::async(std::launch::async, cons);
    auto res3 = std::async(std::launch::async, cons);
    ret1.wait();
    ret2.wait();
    ret3.wait();
    auto V1 = res1.get();
    auto V2 = res2.get();
    auto V3 = res3.get();

    EXPECT_EQ(V1 + V2 + V3, 3'000'000);
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_queue, pop_callback_tests)
{
    BlockingQueue<int64_t> sq;
    int pushcnt = 0;
    auto prod1 = [&]() {
        sq.push(7);
        ++pushcnt;
    };

    auto cons = [&](int cnt) {
        for (int ii = 0; ii < cnt; ii++) {
            sq.popOrCall(prod1);
        }
        return cnt;
    };

    auto res = cons(25);
    EXPECT_EQ(res, 25);
    EXPECT_EQ(pushcnt, 25);
    auto res2 = cons(127);
    EXPECT_EQ(res2, 127);
    EXPECT_EQ(pushcnt, 127 + 25);
}
