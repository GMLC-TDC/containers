/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/

#include "StableBlockDeque.hpp"

#include "gtest/gtest.h"
#include <iostream>
#include <string>

TEST (stackBlockDequeTest, test_simple)
{
    StableBlockDeque<std::string, 2> sbv;

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
    for (const auto &el : sbv)
    {
        (void)(el);
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
