/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "StackBuffer.hpp"

#include "gtest/gtest.h"
#include <iostream>

using namespace gmlc::containers;

TEST(stackQueueTest, test_stackqueueraw_simple)
{
    unsigned char* block = new unsigned char[4096];
    StackBufferRaw stack(block, 4096);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, '\0');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');
    EXPECT_EQ(testData[1023], 0);
    delete[] block;
}

TEST(stackQueueTest, test_stackqueueraw_3_push)
{
    unsigned char* block = new unsigned char[4096];
    StackBufferRaw stack(block, 4096);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'b');
    pushed = stack.push(testData.data(), 249);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'c');
    pushed = stack.push(testData.data(), 393);
    EXPECT_TRUE(pushed);
    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 393);
    EXPECT_EQ(testData[0], 'c');
    EXPECT_EQ(testData[236], 'c');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    EXPECT_TRUE(stack.empty());

    EXPECT_FALSE(stack.push(nullptr, 0));
    delete[] block;
}

TEST(stackQueueTest, test_stackqueueraw_push_full)
{
    unsigned char* block = new unsigned char[1024];
    StackBufferRaw stack(block, 1024);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'b');
    pushed = stack.push(testData.data(), 249);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'c');
    pushed = stack.push(testData.data(), 393);
    EXPECT_TRUE(!pushed);

    EXPECT_TRUE(!stack.isSpaceAvailable(393));
    EXPECT_TRUE(!stack.isSpaceAvailable(200));
    EXPECT_TRUE(stack.isSpaceAvailable(180));
    EXPECT_EQ(stack.getCurrentCount(), 2);

    pushed = stack.push(testData.data(), 180);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'd');
    EXPECT_EQ(stack.getCurrentCount(), 3);

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 180);
    EXPECT_EQ(testData[0], 'c');
    EXPECT_EQ(testData[179], 'c');
    EXPECT_EQ(testData[180], 'd');  // this is one past the copy

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    EXPECT_TRUE(stack.empty());
    EXPECT_EQ(stack.nextDataSize(), 0);
    delete[] block;
}

TEST(stackQueueTest, test_stackqueueraw_reverse)
{
    unsigned char* block = new unsigned char[4096];
    StackBufferRaw stack(block, 4096);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'b');
    pushed = stack.push(testData.data(), 249);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'c');
    pushed = stack.push(testData.data(), 393);
    EXPECT_TRUE(pushed);

    stack.reverse();

    EXPECT_EQ(stack.nextDataSize(), 571);
    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 393);
    EXPECT_EQ(testData[0], 'c');
    EXPECT_EQ(testData[236], 'c');

    EXPECT_TRUE(stack.empty());
    delete[] block;
}

TEST(stackQueueTest, test_stackqueue_clear)
{
    StackBuffer stack(4096);
    std::vector<unsigned char> testData(1024, 'a');

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, '\0');
    pushed = stack.push(testData.data(), 245);
    EXPECT_TRUE(pushed);

    EXPECT_FALSE(stack.empty());
    stack.clear();
    EXPECT_TRUE(stack.empty());
}

TEST(stackQueueTest, test_stackqueue_swap)
{
    StackBuffer stack(4096);

    StackBuffer stack2(1092);
    std::vector<unsigned char> testData(1024, 'a');

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, '\0');
    pushed = stack.push(testData.data(), 245);
    EXPECT_TRUE(pushed);

    stack2.push(testData.data(), 125);
    stack.swap(stack2);

    EXPECT_EQ(stack.getCurrentCount(), 1);
    EXPECT_EQ(stack.nextDataSize(), 125);

    EXPECT_EQ(stack2.getCurrentCount(), 2);
    EXPECT_EQ(stack2.nextDataSize(), 245);
}

TEST(stackQueueTest, test_stackqueue_simple)
{
    StackBuffer stack(4096);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, '\0');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');
    EXPECT_EQ(testData[1023], 0);
}

TEST(stackQueueTest, test_stackqueue_3_push)
{
    StackBuffer stack(4096);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'b');
    pushed = stack.push(testData.data(), 249);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'c');
    pushed = stack.push(testData.data(), 393);
    EXPECT_TRUE(pushed);
    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 393);
    EXPECT_EQ(testData[0], 'c');
    EXPECT_EQ(testData[236], 'c');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    EXPECT_TRUE(stack.empty());
}

TEST(stackQueueTest, test_stackqueue_push_full)
{
    StackBuffer stack(1024);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'b');
    pushed = stack.push(testData.data(), 249);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'c');
    pushed = stack.push(testData.data(), 393);
    EXPECT_TRUE(!pushed);

    EXPECT_TRUE(!stack.isSpaceAvailable(393));
    EXPECT_TRUE(!stack.isSpaceAvailable(200));
    EXPECT_TRUE(stack.isSpaceAvailable(180));
    EXPECT_EQ(stack.getCurrentCount(), 2);

    pushed = stack.push(testData.data(), 180);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'd');
    EXPECT_EQ(stack.getCurrentCount(), 3);

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 180);
    EXPECT_EQ(testData[0], 'c');
    EXPECT_EQ(testData[179], 'c');
    EXPECT_EQ(testData[180], 'd');  // this is one past the copy

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    EXPECT_TRUE(stack.empty());
}

TEST(stackQueueTest, test_stackqueue_reverse)
{
    StackBuffer stack(4096);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'b');
    pushed = stack.push(testData.data(), 249);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'c');
    pushed = stack.push(testData.data(), 393);
    EXPECT_TRUE(pushed);

    stack.reverse();

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 393);
    EXPECT_EQ(testData[0], 'c');
    EXPECT_EQ(testData[236], 'c');

    EXPECT_TRUE(stack.empty());
}

TEST(stackQueueTest, test_stackqueue_move)
{
    StackBuffer stack(2048);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    stack.push(testData.data(), 571);
    testData.assign(1024, '\0');

    StackBuffer mstack(std::move(stack));
    EXPECT_EQ(mstack.getCurrentCount(), 1);
    res = mstack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');
    EXPECT_EQ(testData[1023], 0);
}

TEST(stackQueueTest, test_stackqueue_3_push_and_copy)
{
    StackBuffer stack(4096);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'b');
    pushed = stack.push(testData.data(), 249);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'c');
    pushed = stack.push(testData.data(), 393);
    EXPECT_TRUE(pushed);

    StackBuffer cstack(stack);
    stack.reverse();

    res = cstack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 393);
    EXPECT_EQ(testData[0], 'c');
    EXPECT_EQ(testData[236], 'c');

    res = cstack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());

    res = cstack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    EXPECT_TRUE(cstack.empty());
    EXPECT_TRUE(!stack.empty());

    // check the original still has data and was reversed
    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    // now copy assign the stack
    cstack = stack;

    res = cstack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());
}

TEST(stackQueueTest, test_stackqueue_move_assignement)
{
    StackBuffer stack(2048);
    StackBuffer stack2(1024);
    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    stack.push(testData.data(), 571);
    testData.assign(1024, 'b');
    stack2.push(testData.data(), 397);

    stack2 = std::move(stack);
    testData.assign(1024, '\0');

    EXPECT_EQ(stack2.getCurrentCount(), 1);
    res = stack2.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');
    EXPECT_EQ(testData[1023], 0);
}

TEST(stackQueueTest, test_stackqueue_3_push_resize)
{
    StackBuffer stack(2048);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'b');
    pushed = stack.push(testData.data(), 249);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'c');
    pushed = stack.push(testData.data(), 393);
    EXPECT_TRUE(pushed);
    // make sure to trigger a reallocation in memory
    stack.resize(100000);
    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 393);
    EXPECT_EQ(testData[0], 'c');
    EXPECT_EQ(testData[236], 'c');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    EXPECT_TRUE(stack.empty());
}

TEST(stackQueueTest, test_stackqueue_3_push_resize_shrink)
{
    StackBuffer stack(2048);

    std::vector<unsigned char> testData(1024, 'a');
    int res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 0);

    bool pushed = stack.push(testData.data(), 571);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'b');
    pushed = stack.push(testData.data(), 249);
    EXPECT_TRUE(pushed);
    testData.assign(1024, 'c');
    pushed = stack.push(testData.data(), 393);
    EXPECT_TRUE(pushed);

    stack.resize(1400);

    EXPECT_EQ(stack.capacity(), 1400);

    EXPECT_THROW(stack.resize(95), std::runtime_error);
    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 393);
    EXPECT_EQ(testData[0], 'c');
    EXPECT_EQ(testData[236], 'c');

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 249);
    EXPECT_EQ(testData[0], 'b');
    EXPECT_EQ(testData[236], 'b');
    EXPECT_TRUE(!stack.empty());

    res = stack.pop(testData.data(), 1024);
    EXPECT_EQ(res, 571);
    EXPECT_EQ(testData[0], 'a');
    EXPECT_EQ(testData[236], 'a');
    EXPECT_EQ(testData[570], 'a');

    EXPECT_TRUE(stack.empty());
}

TEST(stackQueueTest, test_stackqueue_resize)
{
    StackBuffer stack(2048);
    EXPECT_EQ(stack.capacity(), 2048);

    stack.resize(4096);
    EXPECT_EQ(stack.capacity(), 4096);

    stack.resize(4096);
    EXPECT_EQ(stack.capacity(), 4096);
    stack.resize(2048);
    EXPECT_EQ(stack.capacity(), 2048);
    EXPECT_EQ(stack.rawBlockCapacity(), 4096);

    EXPECT_FALSE(stack.resize(-262354));  // this should do nothing
}

TEST(stackQueueTest, odd_conditions)
{
    StackBuffer buf(1024);

    std::vector<unsigned char> testData(256, 'a');

    EXPECT_FALSE(buf.push(testData.data(), 0));
    EXPECT_FALSE(buf.push(testData.data(), -15));
    EXPECT_FALSE(buf.push(nullptr, 10));

    EXPECT_TRUE(buf.push(testData.data(), 200));
    EXPECT_EQ(buf.nextDataSize(), 200);

    StackBuffer buf2;

    StackBuffer buf3(std::move(buf2));
    buf3.resize(1024);
    EXPECT_EQ(buf3.capacity(), 1024);
    EXPECT_NO_THROW(buf3.resize(1024));
    EXPECT_EQ(buf3.capacity(), 1024);
    buf3.resize(512);
    EXPECT_EQ(buf3.capacity(), 512);
    buf3.resize(512);
    EXPECT_EQ(buf3.capacity(), 512);
    buf3.resize(1024);
    EXPECT_EQ(buf3.capacity(), 1024);

    StackBuffer buf4;
    EXPECT_TRUE(buf4.empty());
    StackBuffer buf5(buf4);
    EXPECT_TRUE(buf5.empty());
}
