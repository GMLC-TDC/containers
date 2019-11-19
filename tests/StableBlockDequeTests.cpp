/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "StableBlockDeque.hpp"

#include "gtest/gtest.h"
#include <algorithm>
#include <atomic>
#include <iostream>
#include <string>

using namespace gmlc::containers;

TEST(stableBlockDequeTest, test_lookup)
{
    StableBlockDeque<std::string, 2> sbd;
    sbd.push_back("bob");
    sbd.emplace_back("bob2");
    sbd.emplace_back("bob3");
    sbd.emplace_back("bob4");
    sbd.emplace_back("bob5");
    EXPECT_EQ(sbd.size(), 5);
    EXPECT_EQ(sbd[0], "bob");
    EXPECT_EQ(sbd[1], "bob2");
    EXPECT_EQ(sbd[2], "bob3");
    EXPECT_EQ(sbd[3], "bob4");
    EXPECT_EQ(sbd[4], "bob5");
}

TEST(stableBlockDequeTest, test_lookup_front)
{
    StableBlockDeque<std::string, 2> sbd;
    sbd.push_front("bob5");
    sbd.emplace_front("bob4");
    sbd.emplace_front("bob3");
    sbd.emplace_front("bob2");
    sbd.emplace_front("bob");
    EXPECT_EQ(sbd.size(), 5);
    EXPECT_EQ(sbd[0], "bob");
    EXPECT_EQ(sbd[1], "bob2");
    EXPECT_EQ(sbd[2], "bob3");
    EXPECT_EQ(sbd[3], "bob4");
    EXPECT_EQ(sbd[4], "bob5");
}

TEST(stableBlockDequeTest, test_const_lookup)
{
    StableBlockDeque<std::string, 2> sbv;
    sbv.push_back("bob");
    sbv.emplace_back("bob2");
    sbv.emplace_back("bob3");
    sbv.emplace_back("bob4");
    sbv.push_back("bob5");
    const auto &sbvc = sbv;
    EXPECT_EQ(sbvc.size(), 5);
    EXPECT_EQ(sbvc[0], "bob");
    EXPECT_EQ(sbvc[1], "bob2");
    sbv[1] = "bob2_mod";
    EXPECT_EQ(sbvc[1], "bob2_mod");
    EXPECT_EQ(sbvc[2], "bob3");
    EXPECT_EQ(sbvc[3], "bob4");
    EXPECT_EQ(sbvc[4], "bob5");
    EXPECT_FALSE(sbvc.empty());
}

TEST(stableBlockDequeTest, test_iterator)
{
    StableBlockDeque<std::string, 2> sbd;
    sbd.push_back("bob");
    sbd.emplace_back("bob2");
    sbd.emplace_back("bob3");
    sbd.emplace_back("bob4");
    sbd.emplace_back("bob5");
    EXPECT_EQ(sbd.size(), 5);
    auto it = sbd.begin();
    EXPECT_EQ(*it, "bob");
    ++it;
    EXPECT_EQ(*it, "bob2");
    it++;
    EXPECT_EQ(*it, "bob3");
    --it;
    EXPECT_EQ(*it, "bob2");
    it += 1;
    EXPECT_EQ(*it, "bob3");
    it += 2;
    EXPECT_EQ(*it, "bob5");
    it -= 1;
    EXPECT_EQ(*it--, "bob4");
    EXPECT_EQ(*it, "bob3");
    it -= 2;
    EXPECT_EQ(*it, "bob");
    int ii = 0;
    while (it != sbd.end())
    {
        ++ii;
        ++it;
    }
    EXPECT_EQ(ii, 5);
}

TEST(stableBlockDequeTest, test_iterator2)
{
    StableBlockDeque<std::string, 2> sbv;
    sbv.push_back("bob");
    sbv.emplace_back("bob2");
    sbv.emplace_back("bob3");
    sbv.emplace_back("bob4");
    EXPECT_EQ(sbv.size(), 4);
    auto it = sbv.begin();
    EXPECT_EQ(*it, "bob");
    ++it;
    EXPECT_EQ(*it, "bob2");
    it++;
    EXPECT_EQ(*it, "bob3");
    --it;
    EXPECT_EQ(*it, "bob2");
    it += 2;
    EXPECT_EQ(*it--, "bob4");
    EXPECT_EQ(*it, "bob3");
    it -= 2;
    EXPECT_EQ(*it, "bob");
    int ii = 0;
    while (it != sbv.end())
    {
        ++ii;
        ++it;
    }
    EXPECT_EQ(ii, 4);
}

TEST(stableBlockDequeTest, test_const_iterator)
{
    StableBlockDeque<std::string, 2> sbv;
    sbv.push_back("bob");
    sbv.emplace_back("bob2");
    sbv.emplace_back("bob3");
    sbv.emplace_back("bob4");
    sbv.emplace_back("bob5");

    const auto &sbvc = sbv;
    EXPECT_EQ(sbvc.size(), 5);
    auto it = sbvc.begin();
    EXPECT_EQ(*it, "bob");
    ++it;
    EXPECT_EQ(*it, "bob2");
    it++;
    EXPECT_EQ(*it, "bob3");
    --it;
    EXPECT_EQ(*it, "bob2");
    it += 1;
    EXPECT_EQ(*it, "bob3");
    it += 2;
    EXPECT_EQ(*it, "bob5");
    it -= 1;
    EXPECT_EQ(*it--, "bob4");
    EXPECT_EQ(*it, "bob3");
    it -= 2;
    EXPECT_EQ(*it, "bob");
    int ii = 0;
    while (it != sbvc.end())
    {
        ++ii;
        ++it;
    }
    EXPECT_EQ(ii, 5);
}

TEST(stableBlockDequeTest, test_const_iterator2)
{
    StableBlockDeque<std::string, 2> sbv;
    sbv.push_back("bob");
    sbv.emplace_back("bob2");
    sbv.emplace_back("bob3");
    sbv.emplace_back("bob4");
    const auto &sbvc = sbv;
    EXPECT_EQ(sbvc.size(), 4);
    auto it = sbvc.begin();
    EXPECT_EQ(*it, "bob");
    ++it;
    EXPECT_EQ(*it, "bob2");
    it++;
    EXPECT_EQ(*it, "bob3");
    --it;
    EXPECT_EQ(*it, "bob2");
    it += 2;
    EXPECT_EQ(*it--, "bob4");
    EXPECT_EQ(*it, "bob3");
    it -= 2;
    EXPECT_EQ(*it, "bob");
    int ii = 0;
    while (it != sbvc.end())
    {
        ++ii;
        ++it;
    }
    EXPECT_EQ(ii, 4);
}

TEST(stableBlockDequeTest, test_simple)
{
    StableBlockDeque<std::string, 2> sbd;

    sbd.push_back("bob");
    sbd.emplace_back("bob2");
    sbd.emplace_back("bob3");
    sbd.emplace_back("bob4");
    sbd.emplace_back("bob5");
    sbd.emplace_back("bob6");
    sbd.push_back("bob7");
    sbd.emplace_back("bob8");
    sbd.emplace_back("bob9");
    sbd.emplace_back("bob10");
    sbd.push_back("bob11");
    sbd.emplace_back("bob12");
    sbd.emplace_back("bob13");
    sbd.emplace_back("bob14");
    int ii = 0;
    for (const auto &el : sbd)
    {
        (void)(el);
        // std::cout << el << "--";
        ++ii;
    }
    EXPECT_EQ(sbd.size(), 14);
    EXPECT_EQ(ii, 14);

    sbd.pop_back();
    sbd.pop_back();
    sbd.pop_back();
    sbd.pop_back();
    auto &sb2 = sbd;
    ii = 0;
    for (const auto &el : sb2)
    {
        (void)(el);
        ++ii;
    }
    EXPECT_EQ(sbd.size(), 10);
    EXPECT_EQ(sb2.size(), 10);
    EXPECT_EQ(ii, 10);
}

TEST(stableBlockDequeTest, iterator_check)
{
    for (size_t sz = 1; sz < 120; ++sz)
    {
        StableBlockDeque<std::string, 4> sbd(sz);
        const auto &sbdcopy = sbd;
        auto it = sbd.begin();
        size_t ii = 0;
        while (it != sbd.end())
        {
            *it = std::to_string(ii);
            ++it;
            ++ii;
        }
        EXPECT_EQ(ii, sz);
        for (ii = 0; ii < sz; ++ii)
        {
            auto tstr = std::to_string(ii);
            auto res = sbd[ii];
            EXPECT_EQ(tstr, res);
            EXPECT_EQ(tstr, sbdcopy[ii]);
        }
    }
}

TEST(stableBlockDequeTest, test_start)
{
    StableBlockDeque<std::string, 4> sbd(367, "frog");
    EXPECT_EQ(sbd.size(), 367U);

    EXPECT_EQ(sbd[231], "frog");
    EXPECT_EQ(sbd[0], "frog");
    EXPECT_EQ(sbd[366], "frog");

    StableBlockDeque<std::string, 2> sbd2(0, "frog");
    EXPECT_EQ(sbd2.size(), 0U);
    StableBlockDeque<std::string, 2> sbd3(20);
    EXPECT_EQ(sbd3.size(), 20U);
    EXPECT_TRUE(sbd3[19].empty());
    EXPECT_TRUE(sbd3[0].empty());
}

TEST(stableBlockDequeTest, test_front_back)
{
    StableBlockDeque<size_t, 4> sbd(200);
    const auto &sbdcopy = sbd;
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    EXPECT_EQ(sbdcopy.front(), 0U);
    EXPECT_EQ(sbdcopy.back(), 199U);
    for (ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd.front(), ii);
        EXPECT_EQ(sbd.back(), 199 - ii);
        EXPECT_EQ(sbdcopy[0], ii);
        EXPECT_EQ(*sbdcopy.begin(), ii);

        sbd.pop_front();
        sbd.pop_back();
    }

    EXPECT_NO_THROW(sbd.pop_front());
    EXPECT_NO_THROW(sbd.pop_back());
}

TEST(stableBlockDequeTest, move_construct)
{
    StableBlockDeque<size_t, 4> sbd(200);
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    StableBlockDeque<size_t, 4> sbd2(std::move(sbd));
    sbd.~StableBlockDeque();  // call the destructor
    EXPECT_EQ(sbd2.size(), 200);
    for (ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd2.front(), ii);
        EXPECT_EQ(sbd2.back(), 199 - ii);

        sbd2.pop_front();
        sbd2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_construct)
{
    StableBlockDeque<size_t, 4> sbd(200);
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    StableBlockDeque<size_t, 4> sbd2(sbd);

    EXPECT_EQ(sbd2.size(), sbd.size());
    for (ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd2.front(), sbd[ii]);
        EXPECT_EQ(sbd2.back(), sbd[199 - ii]);

        sbd2.pop_front();
        sbd2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_construct_empty)
{
    StableBlockDeque<size_t, 4> sbd;
    EXPECT_TRUE(sbd.empty());
    sbd.clear();  // just test this doesn't blow up or something
    EXPECT_TRUE(sbd.empty());
    StableBlockDeque<size_t, 4> sbd2(sbd);

    EXPECT_EQ(sbd2.size(), sbd.size());
    EXPECT_EQ(sbd2.size(), 0);
    sbd2.clear();
    EXPECT_EQ(sbd2.size(), sbd.size());
    EXPECT_EQ(sbd2.size(), 0);
}

TEST(stableBlockDequeTest, move_assign)
{
    StableBlockDeque<size_t, 4> sbd2(100);
    {
        StableBlockDeque<size_t, 4> sbd(200);
        size_t ii = 0;
        for (auto &val : sbd)
        {
            val = ii++;
        }
        sbd2 = std::move(sbd);
    }
    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd2.front(), ii);
        EXPECT_EQ(sbd2.back(), 199 - ii);

        sbd2.pop_front();
        sbd2.pop_back();
    }
}

TEST(stableBlockDequeTest, assign_move)
{
    std::vector<std::string> vec1(200, std::string(200, 'a'));
    StableBlockDeque<std::string, 5> sbd2(100, std::string(100, 'b'));

    sbd2.move_assign(vec1.begin(), vec1.end());
    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd2.front(), std::string(200, 'a'));
        EXPECT_EQ(sbd2.back(), std::string(200, 'a'));

        sbd2.pop_front();
        sbd2.pop_back();
    }
}

TEST(stableBlockDequeTest, assign_move_to_empty)
{
    std::vector<std::string> vec1(200, std::string(200, 'a'));
    StableBlockDeque<std::string, 5> sbd2;

    sbd2.move_assign(vec1.begin(), vec1.end());
    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd2.front(), std::string(200, 'a'));
        EXPECT_EQ(sbd2.back(), std::string(200, 'a'));

        sbd2.pop_front();
        sbd2.pop_back();
    }
}

TEST(stableBlockDequeTest, assign_move_to_bigger)
{
    std::vector<std::string> vec1(200, std::string(200, 'a'));
    StableBlockDeque<std::string, 5> sbd2(500, std::string(100, 'b'));

    sbd2.move_assign(vec1.begin(), vec1.end());
    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd2.front(), std::string(200, 'a'));
        EXPECT_EQ(sbd2.back(), std::string(200, 'a'));

        sbd2.pop_front();
        sbd2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_assign)
{
    StableBlockDeque<size_t, 4> sbd2(100);

    StableBlockDeque<size_t, 4> sbd(200);
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    sbd2 = sbd;

    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd2.front(), sbd[ii]);
        EXPECT_EQ(sbd2.back(), sbd[199 - ii]);

        sbd2.pop_front();
        sbd2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_assign_from_empty)
{
    StableBlockDeque<size_t, 4> sbd2;

    StableBlockDeque<size_t, 4> sbd(200);
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    sbd2 = sbd;

    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd2.front(), sbd[ii]);
        EXPECT_EQ(sbd2.back(), sbd[199 - ii]);

        sbd2.pop_front();
        sbd2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_assign_to_bigger)
{
    StableBlockDeque<size_t, 4> sbd2(400, 5);

    StableBlockDeque<size_t, 4> sbd(200);
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    sbd2 = sbd;

    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 100; ++ii)
    {
        EXPECT_EQ(sbd2.front(), sbd[ii]);
        EXPECT_EQ(sbd2.back(), sbd[199 - ii]);

        sbd2.pop_front();
        sbd2.pop_back();
    }
}

TEST(stableBlockDequeTest, clear_and_fill)
{
    StableBlockDeque<size_t, 4> sbd(200);
    EXPECT_EQ(sbd.size(), 200U);
    sbd.clear();
    EXPECT_EQ(sbd.size(), 0U);
    for (size_t ii = 0; ii < 200; ++ii)
    {
        sbd.push_back(ii);
        sbd.push_front(ii);
    }
    EXPECT_EQ(sbd.size(), 400U);
}

TEST(stableBlockDequeTest, fill_large_back)
{
    StableBlockDeque<size_t, 4> sbd(200, 999999999UL);
    for (int ii = 0; ii < 10000; ++ii)
    {
        sbd.push_back(ii);
    }

    EXPECT_EQ(sbd.size(), 10200);
    for (int ii = 0; ii < 10000; ++ii)
    {
        sbd.pop_back();
    }
    EXPECT_EQ(sbd.size(), 200);
}

TEST(stableBlockDequeTest, fill_large_front)
{
    StableBlockDeque<size_t, 4> sbd(200, 999999999UL);
    for (int ii = 0; ii < 10000; ++ii)
    {
        sbd.push_front(ii);
    }

    EXPECT_EQ(sbd.size(), 10200);
    for (int ii = 0; ii < 10000; ++ii)
    {
        sbd.pop_front();
    }
    EXPECT_EQ(sbd.size(), 200);
}

TEST(stableBlockDequeTest, iterators)
{
    StableBlockDeque<size_t, 3> sbd;
    EXPECT_TRUE(sbd.begin() == sbd.end());
    const auto &csbd = sbd;
    EXPECT_TRUE(csbd.begin() == csbd.end());
    for (size_t ii = 0; ii < 70; ++ii)
    {
        sbd.push_back(ii);
        size_t cnt = 0;
        for (auto &el : sbd)
        {
            ++cnt;
        }
        EXPECT_EQ(cnt, ii + 1);
        cnt = 0;
        for (const auto &el : csbd)
        {
            ++cnt;
        }
        EXPECT_EQ(cnt, ii + 1);
    }
}

std::atomic<size_t> open_allocs;

/// allocation class that just counts the number of open memory allocations
template <typename T>
class BlockAllocator
{
  public:
    using pointer = T *;
    // Allocate memory
    pointer allocate(size_t count)
    {
        ++open_allocs;
        return static_cast<pointer>(malloc(count * sizeof(T)));
    }

    // Delete memory
    void deallocate(pointer ptr, size_t /* count */)
    {
        --open_allocs;
        free(ptr);
    }
};

TEST(stableBlockDequeTest, shrink_to_fit)
{
    {
        StableBlockDeque<size_t, 3, BlockAllocator<size_t>> sbd(500, 999999U);
        EXPECT_GT(open_allocs.load(), 500 / 8);
        auto current = open_allocs.load();
        sbd.clear();
        EXPECT_EQ(open_allocs.load(), current);
        sbd.shrink_to_fit();
        EXPECT_EQ(open_allocs.load(), 1U);
    }
    EXPECT_EQ(open_allocs.load(), 0U);
}
