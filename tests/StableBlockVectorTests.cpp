/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "StableBlockVector.hpp"

#include "gtest/gtest.h"
#include <algorithm>
#include <atomic>
#include <iostream>
#include <string>

using namespace gmlc::containers;

TEST(stableBlockVectorTest, test_lookup)
{
    StableBlockVector<std::string, 2> sbv;
    EXPECT_TRUE(sbv.empty());
    sbv.push_back("bob");
    sbv.emplace_back("bob2");
    sbv.emplace_back("bob3");
    sbv.emplace_back("bob4");
    sbv.emplace_back("bob5");
    EXPECT_EQ(sbv.size(), 5);
    EXPECT_EQ(sbv[0], "bob");
    EXPECT_EQ(sbv[1], "bob2");
    sbv[1] = "bob2_mod";
    EXPECT_EQ(sbv[1], "bob2_mod");
    EXPECT_EQ(sbv[2], "bob3");
    EXPECT_EQ(sbv[3], "bob4");
    EXPECT_EQ(sbv[4], "bob5");
    EXPECT_FALSE(sbv.empty());
}

TEST(stableBlockVectorTest, test_const_lookup)
{
    StableBlockVector<std::string, 2> sbv;
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

TEST(stableBlockVectorTest, test_iterator)
{
    StableBlockVector<std::string, 2> sbv;
    sbv.push_back("bob");
    sbv.emplace_back("bob2");
    sbv.emplace_back("bob3");
    sbv.emplace_back("bob4");
    sbv.emplace_back("bob5");
    EXPECT_EQ(sbv.size(), 5);
    auto it = sbv.begin();
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
    while (it != sbv.end())
    {
        ++ii;
        ++it;
    }
    EXPECT_EQ(ii, 5);
}

TEST(stableBlockVectorTest, test_iterator2)
{
    StableBlockVector<std::string, 2> sbv;
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

TEST(stableBlockVectorTest, test_const_iterator)
{
    StableBlockVector<std::string, 2> sbv;
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

TEST(stableBlockVectorTest, test_const_iterator2)
{
    StableBlockVector<std::string, 2> sbv;
    sbv.push_back("bob");
    sbv.emplace_back("bob2");
    sbv.emplace_back("bob3");
    sbv.emplace_back("bob4");
    const auto &sbvc = sbv;
    EXPECT_EQ(sbvc.size(), 4);
    EXPECT_EQ(sbvc.back(), "bob4");
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
    EXPECT_FALSE(sbvc.empty());
    EXPECT_FALSE(sbv.empty());
}

TEST(stableBlockVectorTest, test_simple)
{
    StableBlockVector<std::string, 2> sbv;
    std::string bb{"bob7"};
    sbv.push_back("bob");
    sbv.emplace_back("bob2");
    sbv.emplace_back("bob3");
    sbv.emplace_back("bob4");
    sbv.emplace_back("bob5");
    sbv.emplace_back("bob6");
    sbv.push_back(bb);
    EXPECT_EQ(sbv.back(), "bob7");
    sbv.emplace_back("bob8");
    sbv.emplace_back("bob9");
    sbv.emplace_back("bob10");
    sbv.push_back("bob11");
    sbv.emplace_back("bob12");
    sbv.emplace_back("bob13");
    sbv.emplace_back("bob14");
    int ii = 0;
    std::vector<std::string> act;
    for (const auto &el : sbv)
    {
        act.push_back(el);
        ++ii;
    }
    EXPECT_EQ(sbv.size(), 14);
    EXPECT_EQ(ii, 14);

    sbv.pop_back();
    sbv.pop_back();
    sbv.pop_back();
    sbv.pop_back();
    auto &sb2 = sbv;
    ii = 0;
    for (const auto &el : sb2)
    {
        (void)(el);
        ++ii;
    }
    EXPECT_EQ(sbv.size(), 10);
    EXPECT_EQ(sb2.size(), 10);
    EXPECT_EQ(ii, 10);
}

TEST(stableBlockVectorTest, iterator_check)
{
    for (size_t sz = 1; sz < 120; ++sz)
    {
        StableBlockVector<std::string, 4> sbd(sz);
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

TEST(stableBlockVectorTest, test_back)
{
    StableBlockVector<size_t, 4> sbd(100);
    const auto &sbdcopy = sbd;
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    EXPECT_EQ(sbdcopy.back(), 99U);
    for (int jj = 99; jj >= 0; --jj)
    {
        EXPECT_EQ(sbd.back(), static_cast<size_t>(jj));
        EXPECT_EQ(sbdcopy.back(), static_cast<size_t>(jj));
        EXPECT_EQ(*sbdcopy.begin(), 0U);

        sbd.pop_back();
    }

    EXPECT_NO_THROW(sbd.pop_back());
}

TEST(stableBlockVectorTest, constructor1)
{
    StableBlockVector<std::string, 7> sbv{15,
                                          "this is an exciting long string"};
    EXPECT_EQ(sbv.size(), 15);
    for (size_t ii = 0; ii < 15; ++ii)
    {
        EXPECT_EQ(sbv[ii], "this is an exciting long string");
    }
}

TEST(stableBlockVectorTest, constructor0)
{
    StableBlockVector<std::string, 7> sbv{0, "this is an exciting long string"};
    EXPECT_TRUE(sbv.empty());
    EXPECT_EQ(sbv.size(), 0);
}

TEST(stableBlockVectorTest, copy_constructor)
{
    StableBlockVector<std::string, 7> sbv{15,
                                          "this is an exciting long string"};
    auto sbv2 = sbv;
    EXPECT_EQ(sbv2.size(), 15);
    for (size_t ii = 0; ii < 15; ++ii)
    {
        EXPECT_EQ(sbv2[ii], "this is an exciting long string");
    }

    StableBlockVector<std::string, 4> sbv_s;
    auto sbvd = sbv_s;
    EXPECT_TRUE(sbvd.empty());
}

TEST(stableBlockVectorTest, copy_construct_empty)
{
    StableBlockVector<size_t, 4> sbd;
    EXPECT_TRUE(sbd.empty());
    sbd.clear();  // just test this doesn't blow up or something
    EXPECT_TRUE(sbd.empty());
    StableBlockVector<size_t, 4> sbd2(sbd);

    EXPECT_EQ(sbd2.size(), sbd.size());
    EXPECT_EQ(sbd2.size(), 0);
    sbd2.clear();
    EXPECT_EQ(sbd2.size(), sbd.size());
    EXPECT_EQ(sbd2.size(), 0);
}

TEST(stableBlockVectorTest, move_constructor)
{
    StableBlockVector<std::string, 7> sbv{15,
                                          "this is an exciting long string"};
    auto sbv2 = std::move(sbv);
    EXPECT_EQ(sbv2.size(), 15);
    for (size_t ii = 0; ii < 15; ++ii)
    {
        EXPECT_EQ(sbv2[ii], "this is an exciting long string");
    }
    EXPECT_TRUE(sbv.empty());
}

TEST(stableBlockVectorTest, move_assign)
{
    StableBlockVector<size_t, 4> sbd2(100);
    {
        StableBlockVector<size_t, 4> sbd(200);
        size_t ii = 0;
        for (auto &val : sbd)
        {
            val = ii++;
        }
        sbd2 = std::move(sbd);
    }
    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 200; ++ii)
    {
        EXPECT_EQ(sbd2.front(), 0);
        EXPECT_EQ(sbd2.back(), 199 - ii);

        sbd2.pop_back();
    }
}

TEST(stableBlockVectorTest, assign_move)
{
    std::vector<std::string> vec1(200, std::string(200, 'a'));
    StableBlockVector<std::string, 5> sbd2(100, std::string(100, 'b'));

    sbd2.move_assign(vec1.begin(), vec1.end());
    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 200U; ++ii)
    {
        EXPECT_EQ(sbd2.back(), std::string(200, 'a'));

        sbd2.pop_back();
    }
}

TEST(stableBlockVectorTest, assign_move_to_empty)
{
    std::vector<std::string> vec1(200, std::string(200, 'a'));
    StableBlockVector<std::string, 5> sbd2;

    sbd2.move_assign(vec1.begin(), vec1.end());
    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 200; ++ii)
    {
        EXPECT_EQ(sbd2.back(), std::string(200, 'a'));

        sbd2.pop_back();
    }
}

TEST(stableBlockVectorTest, assign_move_to_bigger)
{
    std::vector<std::string> vec1(200, std::string(200, 'a'));
    StableBlockVector<std::string, 5> sbd2(500, std::string(100, 'b'));

    sbd2.move_assign(vec1.begin(), vec1.end());
    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 200; ++ii)
    {
        EXPECT_EQ(sbd2.front(), std::string(200, 'a'));
        EXPECT_EQ(sbd2.back(), std::string(200, 'a'));

        sbd2.pop_back();
    }
}

TEST(stableBlockVectorTest, copy_assign)
{
    StableBlockVector<size_t, 4> sbd2(100);

    StableBlockVector<size_t, 4> sbd(200);
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    sbd2 = sbd;

    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 200; ++ii)
    {
        EXPECT_EQ(sbd2.front(), 0U);
        EXPECT_EQ(sbd2.back(), sbd[199 - ii]);

        sbd2.pop_back();
    }
}

TEST(stableBlockVectorTest, copy_assign_from_empty)
{
    StableBlockVector<size_t, 4> sbd2;

    StableBlockVector<size_t, 4> sbd(200);
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    sbd2 = sbd;

    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 200; ++ii)
    {
        EXPECT_EQ(sbd2.front(), sbd[0]);
        EXPECT_EQ(sbd2.back(), sbd[199 - ii]);

        sbd2.pop_back();
    }
}

TEST(stableBlockVectorTest, copy_assign_to_bigger)
{
    StableBlockVector<size_t, 4> sbd2(400, 5);

    StableBlockVector<size_t, 4> sbd(200);
    size_t ii = 0;
    for (auto &val : sbd)
    {
        val = ii++;
    }
    sbd2 = sbd;

    EXPECT_EQ(sbd2.size(), 200);
    for (size_t ii = 0; ii < 200; ++ii)
    {
        EXPECT_EQ(sbd2.front(), sbd[0]);
        EXPECT_EQ(sbd2.back(), sbd[199 - ii]);

        sbd2.pop_back();
    }
}

TEST(stableBlockVectorTest, clear_and_fill)
{
    StableBlockVector<size_t, 4> sbd(200);
    EXPECT_EQ(sbd.size(), 200U);
    sbd.clear();
    EXPECT_EQ(sbd.size(), 0U);
    for (size_t ii = 0; ii < 400; ++ii)
    {
        sbd.push_back(ii);
    }
    EXPECT_EQ(sbd.size(), 400U);
}

TEST(stableBlockVectorTest, fill_large_back)
{
    StableBlockVector<size_t, 4> sbd(200, 999999999UL);
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

TEST(stableBlockVectorTest, iterators)
{
    StableBlockVector<size_t, 3> sbd;
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

TEST(stableBlockVectorTest, shrink_to_fit)
{
    {
        StableBlockVector<size_t, 3, BlockAllocator<size_t>> sbd(500, 999999U);
        EXPECT_GT(open_allocs.load(), 500 / 8);
        auto current = open_allocs.load();
        sbd.clear();
        EXPECT_EQ(open_allocs.load(), current);
        sbd.shrink_to_fit();
        EXPECT_EQ(open_allocs.load(), 1U);
    }
    EXPECT_EQ(open_allocs.load(), 0U);
}

TEST(stableBlockVectorTest, iterator_tests)
{
    StableBlockVector<std::string, 3> sbd(20);

    for (int ii = 0; ii < 20; ++ii)
    {
        sbd[ii] = std::string(30, 'a' + static_cast<char>(ii));
    }

    auto const &sbd2 = sbd;
    auto it = sbd.begin();
    it = it + 2;
    EXPECT_EQ((*it).front(), 'c');

    auto itc = sbd.begin();
    itc = itc + 2;
    EXPECT_EQ((*itc).front(), 'c');

    it += 7;
    itc += 7;
    EXPECT_EQ(it->front(), 'j');
    EXPECT_EQ(itc->front(), 'j');

    it = it - 4;
    itc = itc - 4;
    EXPECT_EQ(it->front(), 'f');
    EXPECT_EQ(itc->front(), 'f');
    EXPECT_TRUE(it);
    EXPECT_TRUE(itc);
}