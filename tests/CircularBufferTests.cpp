/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "CircularBuffer.hpp"
#include "gtest/gtest.h"
#include <iostream>

using namespace gmlc::containers;

TEST (CircBuff_tests, test_circularbuffraw_simple)
{
    unsigned char *block = new unsigned char[1024];
    CircularBufferRaw buf (block, 1024);

    std::vector<unsigned char> testData (256, 'a');
    int res = buf.pop (testData.data (), 256);
    EXPECT_EQ (res, 0);
    EXPECT_TRUE (buf.empty ());

    bool pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);
    testData.assign (256, '\0');
    EXPECT_FALSE (buf.empty ());
    res = buf.pop (testData.data (), 1024);
    EXPECT_EQ (res, 200);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[126], 'a');
    EXPECT_EQ (testData[199], 'a');
    EXPECT_EQ (testData[200], 0);

    EXPECT_TRUE (buf.empty ());
    delete[] block;
}

TEST (CircBuff_tests, test_circularbuffraw_loop_around)
{
    unsigned char *block = new unsigned char[1024];
    CircularBufferRaw buf (block, 1024);

    std::vector<unsigned char> testData (256, 'a');

    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);
    bool pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);
    pushed = buf.push (testData.data (), 200);
    EXPECT_FALSE (pushed);

    EXPECT_TRUE (!buf.isSpaceAvailable (20));
    int res = buf.pop (testData.data (), 1024);
    EXPECT_EQ (res, 200);
    EXPECT_TRUE (buf.isSpaceAvailable (20));
    pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);

    buf.clear ();
    EXPECT_TRUE (buf.empty ());
    delete[] block;
}

TEST (CircBuff_tests, test_circularbuffraw_loop_around_repeat)
{
    unsigned char *block =
      new unsigned char[1520];  // 3x504+4  otherwise there is a potential
                                // scenario in which 2 500byte messages cannot
                                // fit
    CircularBufferRaw buf (block, 1520);

    std::vector<unsigned char> testData (500, 'a');
    for (int ii = 1; ii <= 500; ++ii)
    {
        bool pushed = buf.push (testData.data (), ii);
        EXPECT_TRUE (pushed);
        pushed = buf.push (testData.data (), ii);
        EXPECT_TRUE (pushed);
        int res = buf.pop (testData.data (), 500);
        EXPECT_EQ (res, ii);
        res = buf.pop (testData.data (), 500);
        EXPECT_EQ (res, ii);
        EXPECT_TRUE (buf.empty ());
    }

    delete[] block;
}

TEST (CircBuff_tests, test_circularbuff_simple)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');
    int res = buf.pop (testData.data (), 256);
    EXPECT_EQ (res, 0);
    EXPECT_TRUE (buf.empty ());

    bool pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);
    testData.assign (256, '\0');
    EXPECT_FALSE (buf.empty ());
    res = buf.pop (testData.data (), 1024);
    EXPECT_EQ (res, 200);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[126], 'a');
    EXPECT_EQ (testData[199], 'a');
    EXPECT_EQ (testData[200], 0);

    EXPECT_TRUE (buf.empty ());
}

TEST (CircBuff_tests, test_circularbuff_loop_around)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');

    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);
    bool pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);
    pushed = buf.push (testData.data (), 200);
    EXPECT_FALSE (pushed);

    EXPECT_TRUE (!buf.isSpaceAvailable (20));
    int res = buf.pop (testData.data (), 1024);
    EXPECT_EQ (res, 200);
    EXPECT_TRUE (buf.isSpaceAvailable (20));
    pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);

    buf.clear ();
    EXPECT_TRUE (buf.empty ());
}

TEST (CircBuff_tests, test_circularbuff_loop_around_repeat)
{
    CircularBuffer buf (1520);

    std::vector<unsigned char> testData (500, 'a');
    for (int ii = 1; ii <= 500; ++ii)
    {
        bool pushed = buf.push (testData.data (), ii);
        EXPECT_TRUE (pushed);
        pushed = buf.push (testData.data (), ii);
        EXPECT_TRUE (pushed);
        int res = buf.pop (testData.data (), 500);
        EXPECT_EQ (res, ii);
        res = buf.pop (testData.data (), 500);
        EXPECT_EQ (res, ii);
        EXPECT_TRUE (buf.empty ());
    }
}

TEST (CircBuff_tests, test_circularbuff_simple_move)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');
    int res = buf.pop (testData.data (), 256);
    EXPECT_EQ (res, 0);
    EXPECT_TRUE (buf.empty ());

    bool pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);
    testData.assign (256, '\0');
    EXPECT_FALSE (buf.empty ());

    CircularBuffer buf2 (std::move (buf));
    res = buf2.pop (testData.data (), 1024);
    EXPECT_EQ (res, 200);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[126], 'a');
    EXPECT_EQ (testData[199], 'a');
    EXPECT_EQ (testData[200], 0);

    EXPECT_TRUE (buf2.empty ());
}

TEST (CircBuff_tests, test_circularbuff_simple_copy)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');
    int res = buf.pop (testData.data (), 256);
    EXPECT_EQ (res, 0);
    EXPECT_TRUE (buf.empty ());

    bool pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);
    testData.assign (256, '\0');
    EXPECT_FALSE (buf.empty ());

    CircularBuffer buf2 (buf);

    res = buf.pop (testData.data (), 1024);
    EXPECT_EQ (res, 200);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[126], 'a');
    EXPECT_EQ (testData[199], 'a');
    EXPECT_EQ (testData[200], 0);

    EXPECT_TRUE (buf.empty ());

    EXPECT_FALSE (buf2.empty ());

    res = buf2.pop (testData.data (), 1024);
    EXPECT_EQ (res, 200);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[126], 'a');
    EXPECT_EQ (testData[199], 'a');
    EXPECT_EQ (testData[200], 0);
}

TEST (CircBuff_tests, test_circularbuff_simple_move_assignment)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');
    int res = buf.pop (testData.data (), 256);
    EXPECT_EQ (res, 0);
    EXPECT_TRUE (buf.empty ());

    bool pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);
    testData.assign (256, '\0');
    EXPECT_FALSE (buf.empty ());

    CircularBuffer buf2 (200);
    buf2.push (testData.data (), 10);
    EXPECT_TRUE (pushed);

    buf2 = std::move (buf);
    res = buf2.pop (testData.data (), 1024);
    EXPECT_EQ (res, 200);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[126], 'a');
    EXPECT_EQ (testData[199], 'a');
    EXPECT_EQ (testData[200], 0);

    EXPECT_TRUE (buf2.empty ());
}

TEST (CircBuff_tests, test_circularbuff_simple_copy_assignment)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');
    int res = buf.pop (testData.data (), 256);
    EXPECT_EQ (res, 0);
    EXPECT_TRUE (buf.empty ());

    bool pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);
    testData.assign (256, '\0');
    EXPECT_TRUE (!buf.empty ());

    CircularBuffer buf2 (200);
    buf2.push (testData.data (), 10);
    EXPECT_TRUE (pushed);

    buf2 = buf;
    EXPECT_EQ (buf2.capacity (), 1024);

    res = buf2.pop (testData.data (), 1024);
    EXPECT_EQ (res, 200);
    EXPECT_EQ (testData[0], 'a');
    EXPECT_EQ (testData[126], 'a');
    EXPECT_EQ (testData[199], 'a');
    EXPECT_EQ (testData[200], 0);

    EXPECT_TRUE (buf2.empty ());

    EXPECT_FALSE (buf.empty ());
}

TEST (CircBuff_tests, test_circularbuff_resize)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');

    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);

    buf.resize (2048);
    auto pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);
    pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);

    EXPECT_EQ (buf.capacity (), 2048);
}

TEST (CircBuff_tests, test_circularbuff_resize_smaller)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');

    buf.push (testData.data (), 200);
    buf.push (testData.data (), 200);

    buf.resize (450);
    auto pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (!pushed);
    int sz = buf.pop (testData.data (), 256);
    EXPECT_EQ (sz, 200);
    pushed = buf.push (testData.data (), 200);
    EXPECT_TRUE (pushed);

    EXPECT_EQ (buf.capacity (), 450);
}

TEST (CircBuff_tests, test_circularbuff_resize_bigger_wrap)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');

    buf.push (testData.data (), 200);
    buf.push (testData.data (), 201);
    buf.push (testData.data (), 202);
    buf.push (testData.data (), 203);

    buf.pop (testData.data (), 256);
    buf.pop (testData.data (), 256);
    buf.push (testData.data (), 204);

    EXPECT_FALSE (buf.isSpaceAvailable (200));
    buf.resize (2048);
    auto pushed = buf.push (testData.data (), 205);
    EXPECT_TRUE (pushed);
    pushed = buf.push (testData.data (), 206);
    EXPECT_TRUE (pushed);
    EXPECT_EQ (buf.pop (testData.data (), 256), 202);
    EXPECT_EQ (buf.pop (testData.data (), 256), 203);
    EXPECT_EQ (buf.pop (testData.data (), 256), 204);
    EXPECT_EQ (buf.pop (testData.data (), 256), 205);
    EXPECT_EQ (buf.pop (testData.data (), 256), 206);
    EXPECT_EQ (buf.capacity (), 2048);
}

TEST (CircBuff_tests, test_circularbuff_resize_smaller_wrap)
{
    CircularBuffer buf (1024);

    std::vector<unsigned char> testData (256, 'a');

    buf.push (testData.data (), 200);
    buf.push (testData.data (), 201);
    buf.push (testData.data (), 202);
    buf.push (testData.data (), 203);

    buf.pop (testData.data (), 256);
    buf.pop (testData.data (), 256);
    buf.push (testData.data (), 204);
    buf.pop (testData.data (), 256);
    EXPECT_TRUE (buf.isSpaceAvailable (205));
    buf.resize (620);  // a size that can work
    EXPECT_TRUE (!buf.isSpaceAvailable (205));
    auto pushed = buf.push (testData.data (), 205);
    EXPECT_TRUE (!pushed);

    EXPECT_THROW (buf.resize (200), std::runtime_error);
    EXPECT_EQ (buf.pop (testData.data (), 256), 203);
    EXPECT_EQ (buf.pop (testData.data (), 256), 204);
    EXPECT_EQ (buf.capacity (), 620);
}

TEST (CircBuff_tests, test_circularbuff_loop_around_repeat_resize)
{
    CircularBuffer buf (45);

    std::vector<unsigned char> testData (10000, 'a');
    for (int ii = 1; ii <= 10000; ++ii)
    {
        buf.resize (3 * (ii + 8));
        int res = buf.pop (testData.data (), 10000);
        EXPECT_EQ (res, ii - 1);
        bool pushed = buf.push (testData.data (), ii);
        EXPECT_TRUE (pushed);
        pushed = buf.push (testData.data (), ii);
        EXPECT_TRUE (pushed);
        res = buf.pop (testData.data (), 10000);
        EXPECT_EQ (res, ii);
        res = buf.pop (testData.data (), 10000);
        EXPECT_EQ (res, ii);
        EXPECT_TRUE (buf.empty ());
        pushed = buf.push (testData.data (), ii);
        EXPECT_TRUE (pushed);
    }
}
