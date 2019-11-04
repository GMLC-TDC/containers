/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "StableBlockDeque.hpp"

#include "gtest/gtest.h"
#include <algorithm>
#include <iostream>
#include <string>

using namespace gmlc::containers;

TEST(stackBlockDequeTest, test_lookup)
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

TEST(stackBlockDequeTest, test_lookup_front)
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

TEST(stackBlockDequeTest, test_iterator)
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

TEST(stackBlockDequeTest, test_iterator2)
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

TEST(stackBlockDequeTest, test_const_iterator)
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

TEST(stackBlockDequeTest, test_const_iterator2)
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

TEST(stackBlockDequeTest, test_simple)
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

TEST(stackBlockDequeTest, iterator_check)
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

TEST(stackBlockDequeTest, test_start)
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

TEST(stackBlockDequeTest, test_front_back)
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
}