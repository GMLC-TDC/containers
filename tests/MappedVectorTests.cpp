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
TEST(mapped_vector_tests, definition_tests)
{
    MappedVector<double> M;
    MappedVector<std::string> S2;
    EXPECT_EQ(M.size(), 0u);
    EXPECT_EQ(S2.size(), 0u);
    MappedVector<std::vector<std::string>, double> V2;

    // test move and assignment operators
    auto V3 = V2;
    decltype(M) TV2;
    TV2 = std::move(M);

    decltype(TV2) TV3;
    TV3 = TV2;
}

TEST(mapped_vector_tests, insertion_tests)
{
    MappedVector<std::vector<double>> Mvec;
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

    auto loc = Mvec.insert("a2", std::vector<double>(14));
    EXPECT_FALSE(loc);

    auto V5 = Mvec.find("not_available");
    EXPECT_EQ(V5, Mvec.end());

    const auto& Mvecc = Mvec;
    auto V4c = Mvecc.find("a2");
    EXPECT_EQ(V4c->size(), 45);

    auto V5c = Mvecc.find("not_available");
    EXPECT_EQ(V5c, Mvecc.end());

    auto& V2c = Mvecc[1];
    EXPECT_EQ(V2c.size(), 45);
    EXPECT_EQ(Mvecc.back().size(), 45);
}

TEST(mapped_vector_tests, insert_or_assign)
{
    MappedVector<std::vector<double>> Mvec;
    Mvec.insert("el1", 3, 1.7);
    EXPECT_EQ(Mvec.size(), 1u);
    auto loca2 = Mvec.insert("a2", std::vector<double>(45));
    EXPECT_TRUE(loca2);
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

    auto loc = Mvec.insert("a2", std::vector<double>(14));
    EXPECT_FALSE(loc);

    auto loca2i = Mvec.insert_or_assign("a2", std::vector<double>(14));
    EXPECT_EQ(loca2i, *loca2);
    auto V5 = Mvec.find("a2");
    EXPECT_EQ(V5->size(), 14);

    EXPECT_EQ(Mvec.back().size(), 14);

    auto loca3i = Mvec.insert_or_assign("a3", std::vector<double>(16));

    EXPECT_EQ(Mvec.back().size(), 16);
    EXPECT_EQ(Mvec[loca3i].size(), 16);
    EXPECT_EQ(loca3i, Mvec.size() - 1);
}

TEST(mapped_vector_tests, insertion_tests_nomap)
{
    MappedVector<std::vector<double>> Mvec;
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

    auto loc = Mvec.insert(no_search, std::vector<double>(22));
    EXPECT_TRUE(loc);
    auto& V4 = Mvec[*loc];
    EXPECT_EQ(V4.size(), 22);
}

TEST(mapped_vector_tests, apply_tests)
{
    MappedVector<double> Mvec;

    Mvec.insert("s1", 3.2);
    Mvec.insert("s2", 4.3);
    Mvec.insert("s3", 9.7);
    Mvec.insert("s4", 11.4);

    EXPECT_EQ(Mvec.size(), 4);

    double sum = 0;
    auto accum = [&sum](double val) { sum += val; };

    Mvec.apply(accum);
    EXPECT_DOUBLE_EQ(sum, 3.2 + 4.3 + 9.7 + 11.4);
    double sum1 = sum;
    sum = 0.0;
    Mvec.transform([](double val) { return val + 1; });

    EXPECT_EQ(Mvec[0], 3.2 + 1.0);
    EXPECT_EQ(Mvec[1], 4.3 + 1.0);
    EXPECT_EQ(Mvec[2], 9.7 + 1.0);

    Mvec.apply(accum);
    EXPECT_DOUBLE_EQ(sum, sum1 + 4.0);
}

TEST(mapped_vector_tests, remove_tests)
{
    MappedVector<double> Mvec;

    Mvec.insert("s1", 3.2);
    Mvec.insert("s2", 4.3);
    Mvec.insert("s3", 9.7);
    Mvec.insert("s4", 11.4);

    EXPECT_EQ(Mvec.size(), 4);

    Mvec.removeIndex(1);

    EXPECT_EQ(Mvec.size(), 3);
    // make sure this does nothing
    Mvec.removeIndex(45);
    EXPECT_EQ(Mvec.size(), 3);
    EXPECT_TRUE(Mvec.find("s2") == Mvec.end());
    EXPECT_EQ(Mvec[1], 9.7);
    EXPECT_EQ(*Mvec.find("s4"), 11.4);

    Mvec.remove("s1");
    EXPECT_EQ(Mvec.size(), 2);
    EXPECT_EQ(*Mvec.find("s4"), 11.4);
    EXPECT_EQ(Mvec[0], 9.7);

    // this should do nothing
    Mvec.remove("s1");
    EXPECT_EQ(Mvec.size(), 2);

    auto MV2 = std::move(Mvec);
    EXPECT_EQ(MV2.size(), 2);

    auto MV3 = MV2;
    EXPECT_EQ(MV2.size(), 2);
    EXPECT_EQ(MV3.size(), 2);

    MV3.clear();
    EXPECT_EQ(MV2.size(), 2);
    EXPECT_EQ(MV3.size(), 0);
}

TEST(mapped_vector_tests, empty_find)
{
    MappedVector<double> Mvec;
    auto res = Mvec.find("string1");
    EXPECT_EQ(res, Mvec.end());
}
