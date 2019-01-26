/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/

#include "StackQueue.hpp"

#include "gtest/gtest.h"
#include <iostream>

using namespace helics::common;

TEST (stackQueueTest, test_stackqueueraw_simple)
{
    unsigned char *block = new unsigned char[4096];
    StackQueueRaw stack (block, 4096);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, '\0');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');
    EXPECT_EQ (testData[1023], 0);
    delete[] block;
}

TEST (stackQueueTest, test_stackqueueraw_3_push)
{
    unsigned char *block = new unsigned char[4096];
    StackQueueRaw stack (block, 4096);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'b');
    pushed = stack.push (testData.data (), 249);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'c');
    pushed = stack.push (testData.data (), 393);
    EXPECT_TRUE (pushed);
    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 393);
    EXPECT_EQ (testData[0], 'c');
    EXPECT_EQ (testData[236], 'c');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    EXPECT_TRUE (stack.empty ());
    delete[] block;
}

TEST (stackQueueTest, test_stackqueueraw_push_full)
{
    unsigned char *block = new unsigned char[1024];
    StackQueueRaw stack (block, 1024);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'b');
    pushed = stack.push (testData.data (), 249);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'c');
    pushed = stack.push (testData.data (), 393);
    EXPECT_TRUE (!pushed);

    EXPECT_TRUE (!stack.isSpaceAvailable (393));
    EXPECT_TRUE (!stack.isSpaceAvailable (200));
    EXPECT_TRUE (stack.isSpaceAvailable (180));
    EXPECT_EQ (stack.getCurrentCount (), 2);

    pushed = stack.push (testData.data (), 180);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'd');
    EXPECT_EQ (stack.getCurrentCount (), 3);

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 180);
    EXPECT_EQ (testData[0], 'c');
    EXPECT_EQ (testData[179], 'c');
    EXPECT_EQ (testData[180], 'd');  // this is one past the copy

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    EXPECT_TRUE (stack.empty ());
    delete[] block;
}

TEST (stackQueueTest, test_stackqueueraw_reverse)
{
    unsigned char *block = new unsigned char[4096];
    StackQueueRaw stack (block, 4096);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'b');
    pushed = stack.push (testData.data (), 249);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'c');
    pushed = stack.push (testData.data (), 393);
    EXPECT_TRUE (pushed);

    stack.reverse ();

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 393);
    EXPECT_EQ (testData[0], 'c');
    EXPECT_EQ (testData[236], 'c');

    EXPECT_TRUE (stack.empty ());
    delete[] block;
}

TEST (stackQueueTest, test_stackqueue_simple)
{
    StackQueue stack (4096);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, '\0');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');
    EXPECT_EQ (testData[1023], 0);
}

TEST (stackQueueTest, test_stackqueue_3_push)
{
    StackQueue stack (4096);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'b');
    pushed = stack.push (testData.data (), 249);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'c');
    pushed = stack.push (testData.data (), 393);
    EXPECT_TRUE (pushed);
    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 393);
    EXPECT_EQ (testData[0], 'c');
    EXPECT_EQ (testData[236], 'c');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    EXPECT_TRUE (stack.empty ());
}

TEST (stackQueueTest, test_stackqueue_push_full)
{
    StackQueue stack (1024);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'b');
    pushed = stack.push (testData.data (), 249);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'c');
    pushed = stack.push (testData.data (), 393);
    EXPECT_TRUE (!pushed);

    EXPECT_TRUE (!stack.isSpaceAvailable (393));
    EXPECT_TRUE (!stack.isSpaceAvailable (200));
    EXPECT_TRUE (stack.isSpaceAvailable (180));
    EXPECT_EQ (stack.getCurrentCount (), 2);

    pushed = stack.push (testData.data (), 180);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'd');
    EXPECT_EQ (stack.getCurrentCount (), 3);

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 180);
    EXPECT_EQ (testData[0], 'c');
    EXPECT_EQ (testData[179], 'c');
    EXPECT_EQ (testData[180], 'd');  // this is one past the copy

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    EXPECT_TRUE (stack.empty ());
}

TEST (stackQueueTest, test_stackqueue_reverse)
{
    StackQueue stack (4096);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'b');
    pushed = stack.push (testData.data (), 249);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'c');
    pushed = stack.push (testData.data (), 393);
    EXPECT_TRUE (pushed);

    stack.reverse ();

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 393);
    EXPECT_EQ (testData[0], 'c');
    EXPECT_EQ (testData[236], 'c');

    EXPECT_TRUE (stack.empty ());
}

TEST (stackQueueTest, test_stackqueue_move)
{
    StackQueue stack (2048);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    stack.push (testData.data (), 571);
    testData.assign (1024, '\0');

    StackQueue mstack (std::move (stack));
    EXPECT_EQ (mstack.getCurrentCount (), 1);
    res = mstack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');
    EXPECT_EQ (testData[1023], 0);
}

TEST (stackQueueTest, test_stackqueue_3_push_and_copy)
{
    StackQueue stack (4096);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'b');
    pushed = stack.push (testData.data (), 249);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'c');
    pushed = stack.push (testData.data (), 393);
    EXPECT_TRUE (pushed);

    StackQueue cstack (stack);
    stack.reverse ();

    res = cstack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 393);
    EXPECT_EQ (testData[0], 'c');
    EXPECT_EQ (testData[236], 'c');

    res = cstack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());

    res = cstack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    EXPECT_TRUE (cstack.empty ());
    EXPECT_TRUE (!stack.empty ());

    // check the original still has data and was reversed
    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    // now copy assign the stack
    cstack = stack;

    res = cstack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());
}

TEST (stackQueueTest, test_stackqueue_move_assignement)
{
    StackQueue stack (2048);
    StackQueue stack2 (1024);
    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    stack.push (testData.data (), 571);
    testData.assign (1024, 'b');
    stack2.push (testData.data (), 397);

    stack2 = std::move (stack);
    testData.assign (1024, '\0');

    EXPECT_EQ (stack2.getCurrentCount (), 1);
    res = stack2.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');
    EXPECT_EQ (testData[1023], 0);
}

TEST (stackQueueTest, test_stackqueue_3_push_resize)
{
    StackQueue stack (2048);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'b');
    pushed = stack.push (testData.data (), 249);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'c');
    pushed = stack.push (testData.data (), 393);
    EXPECT_TRUE (pushed);
    // make sure to trigger a reallocation in memory
    stack.resize (100000);
    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 393);
    EXPECT_EQ (testData[0], 'c');
    EXPECT_EQ (testData[236], 'c');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    EXPECT_TRUE (stack.empty ());
}

TEST (stackQueueTest, test_stackqueue_3_push_resize_shrink)
{
    StackQueue stack (2048);

    std::vector<unsigned char> testData (1024, 'a');
    int res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 0);

    bool pushed = stack.push (testData.data (), 571);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'b');
    pushed = stack.push (testData.data (), 249);
    EXPECT_TRUE (pushed);
    testData.assign (1024, 'c');
    pushed = stack.push (testData.data (), 393);
    EXPECT_TRUE (pushed);

    stack.resize (1400);

    EXPECT_EQ (stack.capacity (), 1400);

    EXPECT_THROW (stack.resize (95), std::runtime_error);
    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 393);
    EXPECT_EQ (testData[0], 'c');
    EXPECT_EQ (testData[236], 'c');

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 249);
    EXPECT_EQ (testData[0], 'b');
    EXPECT_EQ (testData[236], 'b');
    EXPECT_TRUE (!stack.empty ());

    res = stack.pop (testData.data (), 1024);
    EXPECT_EQ (res, 571);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[236], 'a');
    EXPECT_EQ (testData[570], 'a');

    EXPECT_TRUE (stack.empty ());
}
