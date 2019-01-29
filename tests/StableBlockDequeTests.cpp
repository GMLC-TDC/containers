/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC All rights reserved. See LICENSE file and DISCLAIMER
for more details.
*/

#include "StableBlockDeque.hpp"

#include "gtest/gtest.h"
#include <iostream>
#include <string>

using namespace gmlc::containers;

TEST (stackBlockDequeTest, test_lookup)
{
    StableBlockDeque<std::string, 2> sbd;
    sbd.push_back ("bob");
    sbd.emplace_back ("bob2");
    sbd.emplace_back ("bob3");
    sbd.emplace_back ("bob4");
    sbd.emplace_back ("bob5");
    EXPECT_EQ (sbd.size (), 5);
    EXPECT_EQ (sbd[0], "bob");
    EXPECT_EQ (sbd[1], "bob2");
    EXPECT_EQ (sbd[2], "bob3");
    EXPECT_EQ (sbd[3], "bob4");
    EXPECT_EQ (sbd[4], "bob5");
}

TEST (stackBlockDequeTest, test_iterator)
{
    StableBlockDeque<std::string, 2> sbd;
    sbd.push_back ("bob");
    sbd.emplace_back ("bob2");
    sbd.emplace_back ("bob3");
    sbd.emplace_back ("bob4");
    sbd.emplace_back ("bob5");
    EXPECT_EQ (sbd.size (), 5);
    auto it = sbd.begin ();
    EXPECT_EQ (*it, "bob");
    ++it;
    EXPECT_EQ (*it, "bob2");
    it++;
    EXPECT_EQ (*it, "bob3");
    --it;
    EXPECT_EQ (*it, "bob2");
    it += 1;
    EXPECT_EQ (*it, "bob3");
    it += 2;
    EXPECT_EQ (*it, "bob5");
    it -= 1;
    EXPECT_EQ (*it--, "bob4");
    EXPECT_EQ (*it, "bob3");
    it -= 2;
    EXPECT_EQ (*it, "bob");
    int ii = 0;
    while (it != sbd.end ())
    {
        ++ii;
        ++it;
    }
    EXPECT_EQ (ii, 5);
}

TEST (stackBlockDequeTest, test_simple)
{
    StableBlockDeque<std::string, 2> sbd;

    sbd.push_back ("bob");
    sbd.emplace_back ("bob2");
    sbd.emplace_back ("bob3");
    sbd.emplace_back ("bob4");
    sbd.emplace_back ("bob5");
    sbd.emplace_back ("bob6");
    sbd.push_back ("bob7");
    sbd.emplace_back ("bob8");
    sbd.emplace_back ("bob9");
    sbd.emplace_back ("bob10");
    sbd.push_back ("bob11");
    sbd.emplace_back ("bob12");
    sbd.emplace_back ("bob13");
    sbd.emplace_back ("bob14");
    int ii = 0;
    for (const auto &el : sbd)
    {
        (void)(el);
        std::cout << el << "--";
        ++ii;
    }
    EXPECT_EQ (sbd.size (), 14);
    EXPECT_EQ (ii, 14);

    sbd.pop_back ();
    sbd.pop_back ();
    sbd.pop_back ();
    sbd.pop_back ();
    auto &sb2 = sbd;
    ii = 0;
    for (const auto &el : sb2)
    {
        (void)(el);
        ++ii;
    }
    EXPECT_EQ (sbd.size (), 10);
    EXPECT_EQ (sb2.size (), 10);
    EXPECT_EQ (ii, 10);
}
