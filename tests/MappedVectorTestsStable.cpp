/*
Copyright (c) 2017-2020,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "MappedVector.hpp"

#include "gtest/gtest.h"
#include <iostream>
using namespace gmlc::containers;

/** test basic operations */
TEST(stable_mapped_vector_tests, definition_tests)
{
    MappedVector<double, std::string, reference_stability::stable> M;
    MappedVector<std::string, std::string, reference_stability::stable> S2;
    EXPECT_EQ(M.size(), 0u);
    EXPECT_EQ(S2.size(), 0u);
    MappedVector<std::vector<std::string>, double, reference_stability::stable>
        V2;

    // test move and assignment operators
    auto V3 = V2;
    decltype(M) TV2;
    TV2 = std::move(M);

    decltype(TV2) TV3;
    TV3 = TV2;
}

TEST(stable_mapped_vector_tests, insertion_tests)
{
    MappedVector<std::vector<double>, std::string, reference_stability::stable>
        Mvec;
    Mvec.insert("el1", 3, 1.7);
    EXPECT_EQ(Mvec.size(), 1u);
    Mvec.insert("a2", std::vector<double>(45));
    EXPECT_EQ(Mvec.size(), 2u);
    auto& V = Mvec[0];
    EXPECT_EQ(V.size(), 3u);
    EXPECT_EQ(V[0], 1.7);
    EXPECT_EQ(V[2], 1.7);

    auto& V2 = Mvec[1];
    EXPECT_EQ(V2.size(), 45);

    auto V3 = Mvec.find("el1");
    EXPECT_EQ(V3->size(), 3);

    auto V4 = Mvec.find("a2");
    EXPECT_EQ(V4->size(), 45);
}

TEST(stable_mapped_vector_tests, iterator_tests)
{
    MappedVector<double, std::string, reference_stability::stable> Mvec;

    Mvec.insert("s1", 3.2);
    Mvec.insert("s2", 4.3);
    Mvec.insert("s3", 9.7);
    Mvec.insert("s4", 11.4);

    EXPECT_EQ(Mvec.size(), 4);

    Mvec.transform([](double val) { return val + 1; });

    EXPECT_EQ(Mvec[0], 3.2 + 1.0);
    EXPECT_EQ(Mvec[1], 4.3 + 1.0);
    EXPECT_EQ(Mvec[2], 9.7 + 1.0);
}

TEST(stable_mapped_vector_tests, remove_tests)
{
    MappedVector<double, std::string, reference_stability::stable> Mvec;

    Mvec.insert("s1", 3.2);
    Mvec.insert("s2", 4.3);
    Mvec.insert("s3", 9.7);
    Mvec.insert("s4", 11.4);

    EXPECT_EQ(Mvec.size(), 4);

    Mvec.removeIndex(1);

    EXPECT_TRUE(Mvec.find("s2") == Mvec.end());
    EXPECT_EQ(Mvec[1], 4.3);
    EXPECT_EQ(*Mvec.find("s4"), 11.4);

    Mvec.removeIndex(3);
    EXPECT_TRUE(Mvec.find("s4") == Mvec.end());

    Mvec.remove("s1");
    EXPECT_EQ(*Mvec.find("s3"), 9.7);
    EXPECT_EQ(Mvec[0], 3.2);

    auto MV2 = std::move(Mvec);
    EXPECT_EQ(MV2.size(), 3);

    auto MV3 = MV2;
    EXPECT_EQ(MV2.size(), 3);
    EXPECT_EQ(MV3.size(), 3);

    MV3.clear();
    EXPECT_EQ(MV2.size(), 3);
    EXPECT_EQ(MV3.size(), 0);
}

TEST(mapped_vector_tests_stable, empty_find)
{
    MappedVector<double, std::string, reference_stability::stable> Mvec;
    auto res = Mvec.find("string1");
    EXPECT_EQ(res, Mvec.end());
}
