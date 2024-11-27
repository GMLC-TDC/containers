/*
Copyright (c) 2017-2024,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "gtest/gtest.h"
#include <future>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <utility>
/** these test cases test data_block and data_view objects
 */

#include "BlockingPriorityQueue.hpp"
using namespace gmlc::containers;

/** test basic operations */
TEST(blocking_priority_queue, basic_tests)
{
    BlockingPriorityQueue<int> sq;

    sq.push(45);
    sq.push(54);

    EXPECT_TRUE(!sq.empty());

    auto b = sq.try_pop();
    EXPECT_EQ(*b, 45);
    b = sq.try_pop();
    EXPECT_EQ(*b, 54);

    b = sq.try_pop();
    EXPECT_FALSE(b);
    EXPECT_TRUE(sq.empty());

    sq.push(45);
    sq.push(54);

    // check the prioritization is working
    sq.pushPriority(65);

    b = sq.try_pop();
    EXPECT_EQ(*b, 65);
    b = sq.try_pop();
    EXPECT_EQ(*b, 45);
}

/** test with a move only element*/
TEST(blocking_priority_queue, move_only_tests)
{
    BlockingPriorityQueue<std::unique_ptr<double>> sq;

    sq.push(std::make_unique<double>(4534.23));

    auto e2 = std::make_unique<double>(34.234);
    sq.push(std::move(e2));

    EXPECT_TRUE(!sq.empty());

    auto b = sq.try_pop();
    EXPECT_EQ(**b, 4534.23);
    b = sq.try_pop();
    EXPECT_EQ(**b, 34.234);
    e2 = std::make_unique<double>(29.785);
    sq.pushPriority(std::move(e2));

    b = sq.try_pop();
    EXPECT_EQ(**b, 29.785);
    b = sq.try_pop();
    EXPECT_TRUE(!(b));
    EXPECT_TRUE(sq.empty());
}

/** test the ordering with a larger number of inputs*/

TEST(blocking_priority_queue, ordering_tests)
{
    BlockingPriorityQueue<int> sq;

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
    sq.pushPriority(99);
    b = sq.try_pop();
    EXPECT_EQ(*b, 99);
    for (int ii = 7; ii < 20; ++ii) {
        b = sq.try_pop();
        EXPECT_EQ(*b, ii);
    }

    EXPECT_TRUE(sq.empty());
}

TEST(blocking_priority_queue, emplace_tests)
{
    BlockingPriorityQueue<std::pair<int, double>> sq;

    sq.emplace(10, 45.4);
    sq.emplace(11, 34.1);
    sq.emplace(12, 34.2);
    sq.emplacePriority(14, 19.99);

    auto b = sq.try_pop();
    EXPECT_EQ(b->first, 14);
    EXPECT_EQ(b->second, 19.99);

    b = sq.try_pop();
    EXPECT_EQ(b->first, 10);
    EXPECT_EQ(b->second, 45.4);
    b = sq.try_pop();
    EXPECT_EQ(b->first, 11);
    EXPECT_EQ(b->second, 34.1);
}

TEST(blocking_priority_queue, clear_tests)
{
    BlockingPriorityQueue<int64_t> sq;
    sq.push(10);
    sq.push(100);
    sq.push(1000);
    sq.pop();
    sq.push(20);
    sq.push(20);
    sq.pushPriority(9);
    sq.pushPriority(18);
    EXPECT_FALSE(sq.empty());
    sq.clear();
    EXPECT_TRUE(sq.empty());
}

TEST(blocking_priority_queue, multithreaded_tests_wait)
{
    BlockingPriorityQueue<std::pair<int64_t, int64_t>> sq;
    auto t1 = [&sq]() {
        int ii = 0;
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10);
        std::pair<int64_t, int64_t> el{10, 10};
        while (ii < 500) {
            ++ii;
            auto res = sq.pop(std::chrono::milliseconds(10));

            switch (dist(rng)) {
                case 1:
                    break;
                case 2:
                    sq.pushPriority(el);
                    break;
                case 3:
                    if (res) {
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(10));
                    }
                    [[fallthrough]];
                default:
                    sq.push(el);
                    break;
            }
        }
    };
    auto t2 = [&sq]() {
        int ii = 0;
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(1, 10);
        while (ii < 500) {
            ++ii;
            auto res = sq.pop();
            (void)(res);
            switch (dist(rng)) {
                case 1:

                    break;
                case 2:
                    sq.emplacePriority(20, 20);
                    break;
                case 3:
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                    [[fallthrough]];
                default:
                    sq.emplace(30, 30);
                    break;
            }
        }
    };

    auto t3 = [&sq]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1200));
        int ii = 0;
        while (ii++ < 500) {
            sq.emplace(80, 80);
        }
    };

    auto res1 = std::thread(t1);
    auto res2 = std::thread(t2);
    auto res3 = std::thread(t3);
    EXPECT_TRUE(true);
    res1.join();
    res2.join();
    res3.join();
}
/** test with single consumer/single producer*/
TEST(blocking_priority_queue, multithreaded_tests)
{
    BlockingPriorityQueue<int64_t> sq(1010000);

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
                         // catch
                // up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                res = sq.try_pop();
            }
        }
        return cnt;
    };

    auto prodthread = std::thread(prod1);

    auto V = cons();
    EXPECT_EQ(V, 1'010'000);
    prodthread.join();
}

/** test with single consumer / single producer */
TEST(blocking_priority_queue, pop_tests)
{
    BlockingPriorityQueue<int64_t> sq(1010000);

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

    auto prodthread = std::thread(prod1);
    auto V = cons();
    EXPECT_EQ(V, 1'000'000);
    prodthread.join();
}

/** test with multiple consumer/single producer*/
TEST(blocking_priority_queue, multithreaded_tests2)
{
    BlockingPriorityQueue<int64_t> sq(1010000);

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
                         // catch
                // up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                res = sq.try_pop();
            }
        }
        return cnt;
    };

    std::packaged_task<int64_t()> c1(cons);
    std::packaged_task<int64_t()> c2(cons);
    auto res1 = c1.get_future();
    auto res2 = c2.get_future();
    auto c1thread = std::thread(std::move(c1));
    auto c2thread = std::thread(std::move(c2));
    auto prodthread = std::thread(prod1);
    auto V3 = cons();
    auto V1 = res1.get();
    auto V2 = res2.get();

    EXPECT_EQ(V1 + V2 + V3, 1'010'000);
    prodthread.join();
    c1thread.join();
    c2thread.join();
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_priority_queue, multithreaded_tests3)
{
    BlockingPriorityQueue<int64_t> sq;
    sq.reserve(3'010'000);
    for (int64_t ii = 0; ii < 10'000; ++ii) {
        sq.push(ii);
    }
    auto prod1 = [&]() {
        for (int64_t ii = 0; ii < 1'000'000; ++ii) {
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
                         // catch
                // up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                res = sq.try_pop();
            }
        }
        return cnt;
    };

    std::packaged_task<int64_t()> c1(cons);
    std::packaged_task<int64_t()> c2(cons);
    auto res1 = c1.get_future();
    auto res2 = c2.get_future();
    auto c1thread = std::thread(std::move(c1));
    auto c2thread = std::thread(std::move(c2));
    auto prodthread1 = std::thread(prod1);
    auto prodthread2 = std::thread(prod1);
    auto prodthread3 = std::thread(prod1);

    auto V3 = cons();
    auto V1 = res1.get();
    auto V2 = res2.get();

    EXPECT_EQ(V1 + V2 + V3, 3'010'000);

    prodthread1.join();
    prodthread2.join();
    prodthread3.join();
    c1thread.join();
    c2thread.join();
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_priority_queue, multithreaded_tests3_pop)
{
    BlockingPriorityQueue<int64_t> sq;
    sq.reserve(3'010'000);

    auto prod1 = [&]() {
        for (int64_t ii = 0; ii < 1'000'000; ++ii) {
            sq.push(ii);
        }
        sq.push(-1);
    };

    auto cons = [&]() {
        int64_t res{1};
        int64_t cnt{0};
        while (res >= 0) {
            ++cnt;
            res = sq.pop();
        }
        return cnt;
    };

    std::packaged_task<int64_t()> c1(cons);
    std::packaged_task<int64_t()> c2(cons);
    auto res1 = c1.get_future();
    auto res2 = c2.get_future();
    auto c1thread = std::thread(std::move(c1));
    auto c2thread = std::thread(std::move(c2));
    auto prodthread1 = std::thread(prod1);
    auto prodthread2 = std::thread(prod1);
    auto prodthread3 = std::thread(prod1);
    auto V3 = cons();

    prodthread1.join();
    prodthread2.join();
    prodthread3.join();

    auto V1 = res1.get();
    auto V2 = res2.get();

    EXPECT_EQ(V1 + V2 + V3, 3'000'003);

    std::cout << "production complete" << std::endl;
    c1thread.join();
    c2thread.join();
}

/** test with multiple producer/multiple consumer*/
TEST(blocking_priority_queue, pop_callback_tests)
{
    BlockingPriorityQueue<int64_t> sq;
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
