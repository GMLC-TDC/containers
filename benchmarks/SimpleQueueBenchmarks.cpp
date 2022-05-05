/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "SimpleQueue.hpp"

#include <benchmark/benchmark.h>
#include <future>

using namespace gmlc::containers;

template<class X>
class sqFixture : public benchmark::Fixture {
  public:
    SimpleQueue<X> sq;
};

BENCHMARK_TEMPLATE_DEFINE_F(sqFixture, SProdSCons, int64_t)
(benchmark::State& state)
{
    if (state.thread_index == 0) {
        for (int64_t ii = 0; ii < 1000; ++ii) {
            sq.push(ii);
        }
    }
    for (auto _ : state) {
        if (state.thread_index == 0) {
            for (int64_t ii = 1000; ii <= 301000; ++ii) {
                sq.push(ii);
            }
            sq.push(-1);
        } else {
            int cnt = 0;
            while (cnt == 0) {
                auto res = sq.pop();
                if (!res) {  // yield so the producers can catch up
                    std::this_thread::yield();
                    continue;
                }
                if (*res < 0) {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F(sqFixture, SProdSCons)
    ->Threads(2)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE_DEFINE_F(sqFixture, MProdSCons, int64_t)
(benchmark::State& state)
{
    if (state.thread_index == 0) {
        for (int64_t ii = 0; ii < 1'000; ++ii) {
            sq.push(ii);
        }
    }
    for (auto _ : state) {
        if (state.thread_index == 0) {
            int cnt = 0;
            while (cnt < state.threads - 1) {
                auto res = sq.pop();
                if (!res) {  // yield so the producers can catch up
                    std::this_thread::yield();
                    continue;
                }
                if (*res < 0) {
                    ++cnt;
                }
            }
        } else {
            for (int64_t ii = 1'000; ii <= 101000; ++ii) {
                sq.push(ii);
            }
            sq.push(-1);
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F(sqFixture, MProdSCons)
    ->ThreadRange(4, 8)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

#include <mutex>
#include <queue>
template<class X>
class stdqFixture : public benchmark::Fixture {
  public:
    std::queue<X> q;
    std::mutex prot;

    void push(const X& val)
    {
        std::lock_guard<std::mutex> lk(prot);
        q.push(val);
    }

    opt<X> pop()
    {
        std::lock_guard<std::mutex> lk(prot);
        if (q.empty()) {
            return {};
        }
        opt<X> v = q.front();
        q.pop();
        return v;
    }
};

BENCHMARK_TEMPLATE_DEFINE_F(stdqFixture, SProdSCons_std, int64_t)
(benchmark::State& state)
{
    if (state.thread_index == 0) {
        for (int64_t ii = 0; ii < 1000; ++ii) {
            push(ii);
        }
    }
    for (auto _ : state) {
        if (state.thread_index == 0) {
            for (int64_t ii = 1000; ii <= 301000; ++ii) {
                push(ii);
            }
            push(-1);
        } else {
            int cnt = 0;
            while (cnt == 0) {
                auto res = pop();
                if (!res) {  // yield so the producers can catch up
                    std::this_thread::yield();
                    continue;
                }
                if (*res < 0) {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F(stdqFixture, SProdSCons_std)
    ->Threads(2)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE_DEFINE_F(stdqFixture, MProdSCons_std, int64_t)
(benchmark::State& state)
{
    if (state.thread_index == 0) {
        for (int64_t ii = 0; ii < 1'000; ++ii) {
            push(ii);
        }
    }
    for (auto _ : state) {
        if (state.thread_index == 0) {
            int cnt = 0;
            while (cnt < 3) {
                auto res = pop();
                if (!res) {  // yield so the producers can catch up
                    std::this_thread::yield();
                    continue;
                }
                if (*res < 0) {
                    ++cnt;
                }
            }
        } else {
            for (int64_t ii = 1'000; ii <= 101000; ++ii) {
                push(ii);
            }
            push(-1);
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F(stdqFixture, MProdSCons_std)
    ->ThreadRange(4, 8)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

#ifdef ENABLE_BOOST_TYPES
#include <boost/lockfree/queue.hpp>
template<class X>
class blfFixture : public benchmark::Fixture {
  public:
    boost::lockfree::queue<X> q;

    void push(const X& val)
    {
        while (!q.push(val)) {
        }
    }

    opt<X> pop()
    {
        X val;
        if (q.pop(val)) {
            return val;
        }
        return {};
    }
};

BENCHMARK_TEMPLATE_DEFINE_F(blfFixture, SProdSCons_blf, int64_t)
(benchmark::State& state)
{
    if (state.thread_index == 0) {
        for (int64_t ii = 0; ii < 1000; ++ii) {
            push(ii);
        }
    }
    for (auto _ : state) {
        if (state.thread_index == 0) {
            for (int64_t ii = 1000; ii <= 301000; ++ii) {
                push(ii);
            }
            push(-1);
        } else {
            int cnt = 0;
            while (cnt == 0) {
                auto res = pop();
                if (!res) {  // yield so the producers can catch up
                    std::this_thread::yield();
                    continue;
                }
                if (*res < 0) {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F(blfFixture, SProdSCons_blf)
    ->Threads(2)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE_DEFINE_F(blfFixture, MProdSCons_blf, int64_t)
(benchmark::State& state)
{
    if (state.thread_index == 0) {
        for (int64_t ii = 0; ii < 1'000; ++ii) {
            push(ii);
        }
    }
    for (auto _ : state) {
        if (state.thread_index == 0) {
            int cnt = 0;
            while (cnt < state.threads - 1) {
                auto res = pop();
                if (!res) {  // yield so the producers can catch up
                    std::this_thread::yield();
                    continue;
                }
                if (*res < 0) {
                    ++cnt;
                }
            }
        } else {
            for (int64_t ii = 1'000; ii <= 101000; ++ii) {
                push(ii);
            }
            push(-1);
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F(blfFixture, MProdSCons_blf)
    ->ThreadRange(4, 8)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

#include <boost/lockfree/spsc_queue.hpp>
template<class X>
class bspscFixture : public benchmark::Fixture {
  public:
    boost::lockfree::spsc_queue<X, boost::lockfree::capacity<4048>> q;

    void push(const X& val)
    {
        while (!q.push(val)) {
        }
    }

    opt<X> pop()
    {
        if (q.read_available()) {
            opt<X> ret = q.front();
            q.pop();
            return ret;
        }
        return {};
    }
};

BENCHMARK_TEMPLATE_DEFINE_F(bspscFixture, SProdSCons_bspsc, int64_t)
(benchmark::State& state)
{
    if (state.thread_index == 0) {
        for (int64_t ii = 0; ii < 1000; ++ii) {
            push(ii);
        }
    }
    for (auto _ : state) {
        if (state.thread_index == 0) {
            for (int64_t ii = 1000; ii <= 301000; ++ii) {
                push(ii);
            }
            push(-1);
        } else {
            int cnt = 0;
            while (cnt == 0) {
                auto res = pop();
                if (!res) {  // yield so the producers can catch up
                    std::this_thread::yield();
                    continue;
                }
                if (*res < 0) {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F(bspscFixture, SProdSCons_bspsc)
    ->Threads(2)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

#endif

#include <moodycamel/concurrentqueue.h>
template<class X>
class mcFixture : public benchmark::Fixture {
  public:
    moodycamel::ConcurrentQueue<X> q;

    void push(const X& val) { q.enqueue(val); }

    opt<X> pop()
    {
        X v;
        if (q.try_dequeue(v)) {
            return v;
        }
        return {};
    }
};

BENCHMARK_TEMPLATE_DEFINE_F(mcFixture, SProdSCons_mc, int64_t)
(benchmark::State& state)
{
    if (state.thread_index == 0) {
        for (int64_t ii = 0; ii < 1000; ++ii) {
            push(ii);
        }
    }
    for (auto _ : state) {
        if (state.thread_index == 0) {
            for (int64_t ii = 1000; ii <= 301000; ++ii) {
                push(ii);
            }
            push(-1);
        } else {
            int cnt = 0;
            while (cnt == 0) {
                auto res = pop();
                if (!res) {  // yield so the producers can catch up
                    std::this_thread::yield();
                    continue;
                }
                if (*res < 0) {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F(mcFixture, SProdSCons_mc)
    ->Threads(2)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

BENCHMARK_TEMPLATE_DEFINE_F(mcFixture, MProdSCons_mc, int64_t)
(benchmark::State& state)
{
    if (state.thread_index == 0) {
        for (int64_t ii = 0; ii < 1'000; ++ii) {
            push(ii);
        }
    }
    for (auto _ : state) {
        if (state.thread_index != 0) {
            for (int64_t ii = 1'000; ii <= 101000; ++ii) {
                push(ii);
            }
            push(-1);
        } else {
            int cnt = 0;
            while (cnt < state.threads - 1) {
                auto res = pop();
                if (!res) {  // yield so the producers can catch up
                    std::this_thread::yield();
                    continue;
                }
                if (*res < 0) {
                    ++cnt;
                }
            }
        }
    }
}
// Register the function as a benchmark
BENCHMARK_REGISTER_F(mcFixture, MProdSCons_mc)
    ->ThreadRange(4, 8)
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
