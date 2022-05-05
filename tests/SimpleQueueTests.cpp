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
#include <utility>
/** these test cases test data_block and data_view objects
 */

#include "SimpleQueue.hpp"
using namespace gmlc::containers;

/** test basic operations */
TEST(simple_queue_tests, basic_tests)
{
    SimpleQueue<int> sq;

    sq.push(45);
    sq.push(54);

    EXPECT_TRUE(sq.empty() == false);

    EXPECT_EQ(sq.size(), 2);
    auto b = sq.pop();
    EXPECT_EQ(*b, 45);
    b = sq.pop();
    EXPECT_EQ(*b, 54);

    b = sq.pop();
    EXPECT_TRUE(!(b));
    EXPECT_TRUE(sq.empty());
}

/** test with a move only element*/
TEST(simple_queue_tests, move_only_tests)
{
    SimpleQueue<std::unique_ptr<double>> sq;

    sq.push(std::make_unique<double>(4534.23));

    auto e2 = std::make_unique<double>(34.234);
    sq.push(std::move(e2));

    EXPECT_TRUE(sq.empty() == false);

    EXPECT_EQ(sq.size(), 2);
    auto b = sq.pop();
    EXPECT_EQ(**b, 4534.23);
    b = sq.pop();
    EXPECT_EQ(**b, 34.234);

    b = sq.pop();
    EXPECT_TRUE(!(b));
    EXPECT_TRUE(sq.empty());
}

/** test the ordering with a larger number of inputs*/

TEST(simple_queue_tests, ordering_tests)
{
    SimpleQueue<int> sq;

    for (int ii = 1; ii < 10; ++ii) {
        sq.push(ii);
    }

    auto b = sq.pop();
    EXPECT_EQ(*b, 1);
    for (int ii = 2; ii < 7; ++ii) {
        b = sq.pop();
        EXPECT_EQ(*b, ii);
    }
    for (int ii = 10; ii < 20; ++ii) {
        sq.push(ii);
    }
    for (int ii = 7; ii < 20; ++ii) {
        b = sq.pop();
        EXPECT_EQ(*b, ii);
    }

    EXPECT_TRUE(sq.empty());
}

TEST(simple_queue_tests, emplace_tests)
{
    SimpleQueue<std::pair<int, double>> sq;

    sq.emplace(10, 45.4);
    sq.emplace(11, 34.1);
    sq.emplace(12, 34.2);

    EXPECT_EQ(sq.size(), 3);
    auto b = sq.pop();
    EXPECT_EQ(b->first, 10);
    EXPECT_EQ(b->second, 45.4);
    b = sq.pop();
    EXPECT_EQ(b->first, 11);
    EXPECT_EQ(b->second, 34.1);
}

/** test with single consumer/single producer*/
TEST(simple_queue_tests, multithreaded_tests)
{
    SimpleQueue<int64_t> sq(1010000);

    for (int64_t ii = 0; ii < 10'000; ++ii) {
        sq.push(ii);
    }
    auto prod1 = [&]() {
        for (int64_t ii = 10'000; ii < 1'010'000; ++ii) {
            sq.push(ii);
        }
    };

    auto cons = [&]() {
        auto res = sq.pop();
        int64_t cnt = 1;
        while ((res)) {
            auto nres = sq.pop();
            if (!nres) {  // make an additional sleep period so the producer can
                          // catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                nres = sq.pop();
            }
            if (nres) {
                if (*nres > *res) {
                    ++cnt;
                } else {
                    if (*nres > 0) {
                        printf(
                            "%d came before %d\n",
                            static_cast<int>(*nres),
                            static_cast<int>(*res));
                    }
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
    EXPECT_EQ(V, 1'010'000);
}

/** test with multiple consumer/single producer*/
TEST(simple_queue_tests, multithreaded_tests2)
{
    SimpleQueue<int64_t> sq(1010000);

    for (int64_t ii = 0; ii < 10'000; ++ii) {
        sq.push(ii);
    }
    auto prod1 = [&]() {
        for (int64_t ii = 10'000; ii < 2'010'000; ++ii) {
            sq.push(ii);
        }
    };

    auto cons = [&]() {
        auto res = sq.pop();
        int64_t cnt = 0;
        while ((res)) {
            ++cnt;
            res = sq.pop();
            if (!res) {  // make an additional sleep period so the producer can
                         // catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                res = sq.pop();
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

    EXPECT_EQ(V1 + V2 + V3, 2'010'000);
}

/** test with multiple producer/multiple consumer*/
TEST(simple_queue_tests, multithreaded_tests3)
{
    SimpleQueue<int64_t> sq;
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
        auto res = sq.pop();
        int64_t cnt = 0;
        while ((res)) {
            ++cnt;
            res = sq.pop();
            if (!res) {  // make an additional sleep period so the producer can
                         // catch up
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                res = sq.pop();
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
    ret1.wait();
    ret2.wait();
    ret3.wait();
    auto V1 = res1.get();
    auto V2 = res2.get();
    auto V3 = res3.get();

    EXPECT_EQ(V1 + V2 + V3, 3'010'000);
}

/** test with multiple producer/multiple consumer*/
TEST(simple_queue_tests, move_construct)
{
    SimpleQueue<int64_t> sq;
    sq.push(54);
    sq.push(55);
    SimpleQueue<int64_t> sq2(std::move(sq));

    auto res = sq2.pop();
    EXPECT_EQ(*res, 54);
    res = sq2.pop();
    EXPECT_EQ(*res, 55);
    res = sq2.pop();
    EXPECT_FALSE(res);
}

/** test with multiple producer/multiple consumer*/
TEST(simple_queue_tests, move_assign)
{
    SimpleQueue<int64_t> sq;
    sq.push(54);
    sq.push(55);
    SimpleQueue<int64_t> sq2;
    sq2 = std::move(sq);

    auto res2 = sq2.peek();
    EXPECT_EQ(*res2, 54);
    auto res = sq2.pop();
    EXPECT_EQ(*res, 54);

    res = sq2.pop();
    EXPECT_EQ(*res, 55);
    res = sq2.pop();
    EXPECT_FALSE(res);
    res2 = sq2.peek();
    EXPECT_FALSE(res2);
}

/** test with multiple producer/multiple consumer*/
TEST(simple_queue_tests, pop_test)
{
    SimpleQueue<int64_t> sq;
    sq.push(54);
    sq.push(55);
    sq.push(56);
    sq.pushVector(std::vector<int64_t>{57, 58, 59});
    sq.pop();
    sq.pop();
    sq.push(60);
    EXPECT_TRUE(sq.pop());
    EXPECT_TRUE(sq.pop());
    EXPECT_TRUE(sq.pop());
    EXPECT_TRUE(sq.pop());
    EXPECT_TRUE(sq.pop());
}
