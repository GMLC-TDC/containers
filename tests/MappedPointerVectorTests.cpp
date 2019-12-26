/*
Copyright (c) 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "MappedPointerVector.hpp"

#include "gtest/gtest.h"
#include <iostream>
using namespace gmlc::containers;

/** test basic operations */
TEST(mapped_vector_tests, definition_tests)
{
    MappedPointerVector<double> M;
    MappedPointerVector<std::string> S2;
    EXPECT_EQ(M.size(), 0u);
    EXPECT_EQ(S2.size(), 0u);
    MappedPointerVector<std::vector<std::string>, double> V2;

    // test move operators
    decltype(M) TV2;
    TV2 = std::move(M);

    decltype(TV2) TV3(std::move(TV2));
}

TEST(mapped_vector_tests, insertion_tests)
{
    MappedPointerVector<std::vector<double>> Mvec;
    Mvec.insert("el1", 3, 1.7);
    EXPECT_EQ(Mvec.size(), 1);
    Mvec.insert("a2", std::vector<double>(45));
    EXPECT_EQ(Mvec.size(), 2);
    auto V = Mvec[0];
    EXPECT_EQ(V->size(), 3);
    EXPECT_EQ((*V)[0], 1.7);
    EXPECT_EQ((*V)[2], 1.7);

    auto V2 = Mvec[1];
    EXPECT_EQ(V2->size(), 45);

    auto V3 = Mvec.find("el1");
    EXPECT_EQ(V3->size(), 3);

    auto V4 = Mvec.find("a2");
    EXPECT_EQ(V4->size(), 45);
}

TEST(mapped_vector_tests, iterator_tests)
{
    MappedPointerVector<double> Mvec;

    Mvec.insert("s1", 3.2);
    Mvec.insert("s2", 4.3);
    Mvec.insert("s3", 9.7);
    Mvec.insert("s4", 11.4);

    EXPECT_EQ(Mvec.size(), 4);

    Mvec.apply([](double* val) { *val = *val + 1; });

    EXPECT_EQ(*Mvec[0], 3.2 + 1.0);
    EXPECT_EQ(*Mvec[1], 4.3 + 1.0);
    EXPECT_EQ(*Mvec[2], 9.7 + 1.0);
}

TEST(mapped_vector_tests, remove_tests)
{
    MappedPointerVector<double> Mvec;

    Mvec.insert("s1", 3.2);
    Mvec.insert("s2", 4.3);
    Mvec.insert("s3", 9.7);
    Mvec.insert("s4", 11.4);

    EXPECT_EQ(Mvec.size(), 4);

    Mvec.removeIndex(1);

    EXPECT_EQ(Mvec.size(), 3);
    EXPECT_TRUE(Mvec.find("s2") == nullptr);
    EXPECT_EQ(*Mvec[1], 9.7);
    EXPECT_EQ(*Mvec.find("s4"), 11.4);

    Mvec.remove("s1");
    EXPECT_EQ(Mvec.size(), 2);
    EXPECT_EQ(*Mvec.find("s4"), 11.4);
    EXPECT_EQ(*Mvec[0], 9.7);

    auto MV2 = std::move(Mvec);
    EXPECT_EQ(MV2.size(), 2);

    MV2.clear();
    EXPECT_EQ(MV2.size(), 0);
}
