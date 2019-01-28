/*
Copyright © 2017-2018,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/

#include "SimpleQueue.hpp"
#include <benchmark/benchmark.h>
#include <future>

using namespace gmlc::containers;

template <class X>
class sqFixture : public benchmark::Fixture
{
  public:
    SimpleQueue<X> sq;
};

BENCHMARK_TEMPLATE_DEFINE_F (sqFixture, SProdSCons, int64_t) (benchmark::State &state)
{
    if (state.thread_index == 0)
    {
        for (int64_t ii = 0; ii < 1000; ++ii)
        {
            sq.push (ii);
        }
    }
    for (auto _ : state)
    {
        if (state.thread_index == 0)
        {
            for (int64_t ii = 1000; ii <= 301000; ++ii)
            {
                sq.push (ii);
            }
            sq.push (-1);
        }
        else
        {
            int cnt = 0;
            while (cnt == 0)
            {
                auto res = sq.pop ();
                if (!res)
                {  // yield so the producers can catch up
                    std::this_thread::yield ();
                    continue;
                }
                if (*res < 0)
                {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F (sqFixture, SProdSCons)->Threads (2)->UseRealTime ()->Unit (benchmark::kMillisecond);

BENCHMARK_TEMPLATE_DEFINE_F (sqFixture, MProdSCons, int64_t) (benchmark::State &state)
{
    if (state.thread_index == 0)
    {
        for (int64_t ii = 0; ii < 1'000; ++ii)
        {
            sq.push (ii);
        }
    }
    for (auto _ : state)
    {
        if (state.thread_index < 3)
        {
            for (int64_t ii = 1'000; ii <= 101000; ++ii)
            {
                sq.push (ii);
            }
            sq.push (-1);
        }
        else
        {
            int cnt = 0;
            while (cnt < 3)
            {
                auto res = sq.pop ();
                if (!res)
                {  // yield so the producers can catch up
                    std::this_thread::yield ();
                    continue;
                }
                if (*res < 0)
                {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F (sqFixture, MProdSCons)->Threads (4)->UseRealTime ()->Unit (benchmark::kMillisecond);

#include <mutex>
#include <queue>
template <class X>
class stdqFixture : public benchmark::Fixture
{
  public:
    std::queue<X> q;
    std::mutex prot;

    void push (const X &val)
    {
        std::lock_guard<std::mutex> lk (prot);
        q.push (val);
    }

    opt<X> pop ()
    {
        std::lock_guard<std::mutex> lk (prot);
        if (q.empty ())
        {
            return {};
        }
        opt<X> v = q.front ();
        q.pop ();
        return v;
    }
};

BENCHMARK_TEMPLATE_DEFINE_F (stdqFixture, SProdSCons_std, int64_t) (benchmark::State &state)
{
    if (state.thread_index == 0)
    {
        for (int64_t ii = 0; ii < 1000; ++ii)
        {
            push (ii);
        }
    }
    for (auto _ : state)
    {
        if (state.thread_index == 0)
        {
            for (int64_t ii = 1000; ii <= 301000; ++ii)
            {
                push (ii);
            }
            push (-1);
        }
        else
        {
            int cnt = 0;
            while (cnt == 0)
            {
                auto res = pop ();
                if (!res)
                {  // yield so the producers can catch up
                    std::this_thread::yield ();
                    continue;
                }
                if (*res < 0)
                {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F (stdqFixture, SProdSCons_std)->Threads (2)->UseRealTime ()->Unit (benchmark::kMillisecond);

BENCHMARK_TEMPLATE_DEFINE_F (stdqFixture, MProdSCons_std, int64_t) (benchmark::State &state)
{
    if (state.thread_index == 0)
    {
        for (int64_t ii = 0; ii < 1'000; ++ii)
        {
            push (ii);
        }
    }
    for (auto _ : state)
    {
        if (state.thread_index < 3)
        {
            for (int64_t ii = 1'000; ii <= 101000; ++ii)
            {
                push (ii);
            }
            push (-1);
        }
        else
        {
            int cnt = 0;
            while (cnt < 3)
            {
                auto res = pop ();
                if (!res)
                {  // yield so the producers can catch up
                    std::this_thread::yield ();
                    continue;
                }
                if (*res < 0)
                {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F (stdqFixture, MProdSCons_std)->Threads (4)->UseRealTime ()->Unit (benchmark::kMillisecond);
/*
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
    unsigned char *block = new unsigned char[1520];  // 3x504+4  otherwise there is a potential scenario in which
2
                                                     // 500byte messages cannot fit
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

*/
