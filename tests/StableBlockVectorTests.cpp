/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/

#include "StableBlockVector.hpp"

#include "gtest/gtest.h"
#include <iostream>

#include <string>

using namespace gmlc::containers;

TEST (stackBlockVectorTest, test_lookup)
{
    StableBlockVector<std::string, 2> sbv;
    sbv.push_back ("bob");
    sbv.emplace_back ("bob2");
    sbv.emplace_back ("bob3");
    sbv.emplace_back ("bob4");
    sbv.emplace_back ("bob5");
    EXPECT_EQ (sbv.size (), 5);
    EXPECT_EQ (sbv[0], "bob");
    EXPECT_EQ (sbv[1], "bob2");
    EXPECT_EQ (sbv[2], "bob3");
    EXPECT_EQ (sbv[3], "bob4");
    EXPECT_EQ (sbv[4], "bob5");
}

TEST (stackBlockVectorTest, test_iterator)
{
    StableBlockVector<std::string, 2> sbv;
    sbv.push_back ("bob");
    sbv.emplace_back ("bob2");
    sbv.emplace_back ("bob3");
    sbv.emplace_back ("bob4");
    sbv.emplace_back ("bob5");
    EXPECT_EQ (sbv.size (), 5);
    auto it = sbv.begin ();
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
    while (it != sbv.end ())
    {
        ++ii;
        ++it;
    }
    EXPECT_EQ (ii, 5);
}

TEST (stackBlockVectorTest, test_simple)
{
    StableBlockVector<std::string, 2> sbv;

    sbv.push_back ("bob");
    sbv.emplace_back ("bob2");
    sbv.emplace_back ("bob3");
    sbv.emplace_back ("bob4");
    sbv.emplace_back ("bob5");
    sbv.emplace_back ("bob6");
    sbv.push_back ("bob7");
    sbv.emplace_back ("bob8");
    sbv.emplace_back ("bob9");
    sbv.emplace_back ("bob10");
    sbv.push_back ("bob11");
    sbv.emplace_back ("bob12");
    sbv.emplace_back ("bob13");
    sbv.emplace_back ("bob14");
    int ii = 0;
    std::vector<std::string> act;
    for (const auto &el : sbv)
    {
        act.push_back (el);
        ++ii;
    }
    EXPECT_EQ (sbv.size (), 14);
    EXPECT_EQ (ii, 14);

    sbv.pop_back ();
    sbv.pop_back ();
    sbv.pop_back ();
    sbv.pop_back ();
    auto &sb2 = sbv;
    ii = 0;
    for (const auto &el : sb2)
    {
        (void)(el);
        ++ii;
    }
    EXPECT_EQ (sbv.size (), 10);
    EXPECT_EQ (sb2.size (), 10);
    EXPECT_EQ (ii, 10);
}
