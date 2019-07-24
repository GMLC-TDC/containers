/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "StableBlockVector.hpp"

#include "gtest/gtest.h"
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
    auto sbv2{sbv};
    EXPECT_EQ(sbv2.size(), 15);
    for (size_t ii = 0; ii < 15; ++ii)
    {
        EXPECT_EQ(sbv2[ii], "this is an exciting long string");
    }

    StableBlockVector<std::string, 4> sbv_s;
    auto sbvd{sbv_s};
    EXPECT_TRUE(sbvd.empty());
}

TEST(stableBlockVectorTest, move_constructor)
{
    StableBlockVector<std::string, 7> sbv{15,
                                          "this is an exciting long string"};
    auto sbv2{std::move(sbv)};
    EXPECT_EQ(sbv2.size(), 15);
    for (size_t ii = 0; ii < 15; ++ii)
    {
        EXPECT_EQ(sbv2[ii], "this is an exciting long string");
    }
    EXPECT_TRUE(sbv.empty());
}
