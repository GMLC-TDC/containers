/*
Copyright (c) 2017-2024,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "DualMappedVector.hpp"

#include "gtest/gtest.h"
#include <iostream>
#include <vector>
#include <utility>
#include <string>

using gmlc::containers::DualMappedVector;

/** test basic operations */
TEST(dual_mapped_vector_tests, definition_tests)
{
    DualMappedVector<double, std::string, int64_t> M;
    DualMappedVector<std::string, std::string, double> S2;
    EXPECT_EQ(M.size(), 0u);
    EXPECT_EQ(S2.size(), 0u);
    DualMappedVector<std::vector<std::string>, double, std::string> V2;

    // test move and assignment operators
    decltype(M) TV2;
    TV2 = std::move(M);

    decltype(TV2) TV3;
    TV3 = TV2;
}

TEST(dual_mapped_vector_tests, insertion_tests)
{
    DualMappedVector<std::vector<double>, std::string, int64_t> Mvec;
    Mvec.insert("el1", 41, 3, 1.7);
    EXPECT_EQ(Mvec.size(), 1u);
    Mvec.insert("a2", 27, std::vector<double>(45));
    EXPECT_EQ(Mvec.size(), 2u);
    auto& V = Mvec[0];
    EXPECT_EQ(V.size(), 3u);
    EXPECT_EQ(V[0], 1.7);
    EXPECT_EQ(V[2], 1.7);

    auto& V2 = Mvec[1];
    EXPECT_EQ(V2.size(), 45u);

    auto V3 = Mvec.find("el1");
    EXPECT_EQ(V3->size(), 3u);

    auto V4 = Mvec.find("a2");
    EXPECT_EQ(V4->size(), 45u);

    auto V5 = Mvec.find(41);
    EXPECT_EQ(V5->size(), 3u);
}

TEST(dual_mapped_vector_tests, insertion_tests2)
{
    DualMappedVector<double, std::string, int64_t> Mvec;

    auto res = Mvec.insert("el1", 0, 1.7);
    EXPECT_TRUE(res);
    res = Mvec.insert("el2", 1, 3.4);
    EXPECT_TRUE(res);
    res = Mvec.insert("el2", 1, 22.22);
    EXPECT_FALSE(res);

    res = Mvec.insert("el3", no_search, 5.1);
    EXPECT_TRUE(res);

    res = Mvec.insert("el3", no_search, 9.8);
    EXPECT_FALSE(res);

    res = Mvec.insert(no_search, 3, 9.9);
    EXPECT_TRUE(res);

    res = Mvec.insert(no_search, 3, 14.7);
    EXPECT_FALSE(res);

    res = Mvec.insert(no_search, no_search, 99.9);
    EXPECT_TRUE(res);
    EXPECT_EQ(Mvec[*res], 99.9);
}

TEST(dual_mapped_vector_tests, assign_tests2)
{
    DualMappedVector<double, std::string, int64_t> Mvec;

    Mvec.insert("el1", 0, 1.7);

    auto loc = Mvec.insert("el2", 1, 3.4);

    auto loc2 = Mvec.insert_or_assign("el2", 1, 22.22);
    EXPECT_EQ(loc2, *loc);

    loc2 = Mvec.insert_or_assign("el3", no_search, 5.1);

    auto loc3 = Mvec.insert_or_assign("el3", no_search, 9.8);
    EXPECT_EQ(loc2, loc3);
    EXPECT_EQ(Mvec[loc3], 9.8);

    auto loc4 = Mvec.insert_or_assign(no_search, 3, 9.9);
    EXPECT_GT(loc4, loc3);

    auto loc5 = Mvec.insert_or_assign(no_search, 3, 14.7);
    EXPECT_EQ(loc5, loc4);
    EXPECT_EQ(Mvec[loc5], 14.7);
}

TEST(dual_mapped_vector_tests, additional_searchTerm_tests)
{
    DualMappedVector<double, std::string, int64_t> Mvec;

    Mvec.insert("s1", 64, 3.2);
    Mvec.insert("s2", 63, 4.3);
    Mvec.insert("s3", 47, 9.7);
    Mvec.insert("s4", 92, 11.4);

    EXPECT_EQ(Mvec.size(), 4u);

    Mvec.addSearchTerm("s5", "s1");
    EXPECT_EQ(Mvec.size(), 4u);

    EXPECT_EQ(*(Mvec.find("s5")), 3.2);

    Mvec.addSearchTerm(93, 47);
    EXPECT_EQ(*(Mvec.find(93)), 9.7);

    Mvec.addSearchTerm(143, "s3");
    EXPECT_EQ(*(Mvec.find(143)), 9.7);

    Mvec.addSearchTerm("extra", 63);
    EXPECT_EQ(*(Mvec.find("extra")), 4.3);

    Mvec.addSearchTermForIndex("astring", 3);
    EXPECT_EQ(*(Mvec.find("astring")), 11.4);

    auto res = Mvec.addSearchTermForIndex(99, 2);
    EXPECT_EQ(*(Mvec.find(99)), 9.7);
    EXPECT_TRUE(res);

    res = Mvec.addSearchTermForIndex(99, 99);
    EXPECT_FALSE(res);

    res = Mvec.addSearchTermForIndex("bob", 127);
    EXPECT_FALSE(res);

    // check for appropriate return values
    EXPECT_TRUE(!Mvec.addSearchTerm("missing", "none"));
    EXPECT_TRUE(!Mvec.addSearchTerm(1241, 98));
    EXPECT_TRUE(!Mvec.addSearchTerm("missing", 98));
    EXPECT_TRUE(!Mvec.addSearchTerm(1241, "none"));
}

TEST(dual_mapped_vector_tests, iterator_tests)
{
    DualMappedVector<double, std::string, int64_t> Mvec;

    Mvec.insert("s1", 64, 3.2);
    Mvec.insert("s2", 63, 4.3);
    Mvec.insert("s3", 47, 9.7);
    Mvec.insert("s4", 92, 11.4);

    EXPECT_EQ(Mvec.size(), 4u);

    Mvec.transform([](double val) { return val + 1; });

    EXPECT_EQ(Mvec[0], 3.2 + 1.0);
    EXPECT_EQ(Mvec[1], 4.3 + 1.0);
    EXPECT_EQ(Mvec[2], 9.7 + 1.0);

    double sum = 0.0;
    for (auto& el : Mvec) {
        sum += el;
    }
    EXPECT_EQ(sum, 4.2 + 5.3 + 10.7 + 12.4);
}

TEST(dual_mapped_vector_tests, remove_tests)
{
    DualMappedVector<double, std::string, int64_t> Mvec;

    Mvec.insert("s1", 64, 3.2);
    Mvec.insert("s2", 63, 4.3);
    Mvec.insert("s3", 47, 9.7);
    Mvec.insert("s4", 92, 11.4);

    Mvec.addSearchTerm("s5", 64);
    Mvec.addSearchTermForIndex(107, 2);
    EXPECT_EQ(Mvec.size(), 4u);

    EXPECT_FALSE(Mvec.addSearchTermForIndex("test", 207));

    EXPECT_FALSE(Mvec.addSearchTermForIndex(99, 207));
    Mvec.removeIndex(1);

    EXPECT_EQ(Mvec.size(), 3u);
    // this next one should do nothing
    Mvec.removeIndex(99);
    EXPECT_EQ(Mvec.size(), 3u);

    EXPECT_TRUE(Mvec.find("s2") == Mvec.end());
    EXPECT_EQ(Mvec[1], 9.7);
    EXPECT_EQ(*Mvec.find("s4"), 11.4);
    EXPECT_EQ(*Mvec.find("s5"), 3.2);
    Mvec.remove("s1");
    EXPECT_EQ(Mvec.size(), 2u);

    Mvec.remove("s1");
    EXPECT_EQ(Mvec.size(), 2u);

    EXPECT_EQ(*Mvec.find("s4"), 11.4);
    EXPECT_EQ(Mvec[0], 9.7);
    EXPECT_TRUE(Mvec.find("s5") == Mvec.end());
    EXPECT_EQ(*Mvec.find(107), 9.7);

    auto MV2 = std::move(Mvec);
    EXPECT_EQ(MV2.size(), 2u);

    auto MV3 = MV2;
    EXPECT_EQ(MV2.size(), 2u);
    EXPECT_EQ(MV3.size(), 2u);

    MV3.remove(92);
    EXPECT_EQ(MV2.size(), 2u);
    MV3.remove(92);
    EXPECT_EQ(MV2.size(), 2u);
    EXPECT_EQ(MV3.size(), 1u);
    MV3.clear();
    EXPECT_EQ(MV2.size(), 2u);
    EXPECT_EQ(MV3.size(), 0u);
}

TEST(dual_mapped_vector_tests, const_find_tests)
{
    DualMappedVector<double, std::string, int64_t> Mvec;

    Mvec.insert("s1", 64, 3.2);
    Mvec.insert("s2", 63, 4.3);
    Mvec.insert("s3", 47, 9.7);
    Mvec.insert("s4", 92, 11.4);

    const auto& mv2 = Mvec;
    auto res1 = Mvec.find("s1");
    auto res2 = mv2.find("s1");
    EXPECT_EQ(res1, res2);
    EXPECT_NE(res1, Mvec.end());
    EXPECT_EQ(*res1, 3.2);
    EXPECT_EQ(*res2, 3.2);

    auto r1 = Mvec.find("s7");
    auto r2 = mv2.find("s7");
    EXPECT_EQ(r1, r2);
    EXPECT_EQ(r1, Mvec.end());
    EXPECT_EQ(r2, Mvec.end());
    EXPECT_TRUE(r1 == mv2.end());
    EXPECT_TRUE(r2 == mv2.end());
    EXPECT_TRUE(mv2.end() == r1);
    EXPECT_TRUE(mv2.end() == r2);

    res1 = Mvec.find(63);
    res2 = mv2.find(63);
    EXPECT_EQ(res1, res2);
    EXPECT_NE(res1, Mvec.end());
    EXPECT_EQ(*res1, 4.3);
    EXPECT_EQ(*res2, 4.3);

    r1 = Mvec.find(99);
    r2 = mv2.find(99);
    EXPECT_EQ(r1, r2);
    EXPECT_EQ(r1, Mvec.end());
    EXPECT_EQ(r2, Mvec.end());
    EXPECT_TRUE(r1 == mv2.end());
    EXPECT_TRUE(r2 == mv2.end());
    EXPECT_TRUE(mv2.end() == r1);
    EXPECT_TRUE(mv2.end() == r2);
}

TEST(mapped_vector_tests, apply_tests)
{
    DualMappedVector<double, std::string, int64_t> Mvec;

    Mvec.insert("s1", 1, 3.2);
    Mvec.insert("s2", 2, 4.3);
    Mvec.insert("s3", 3, 9.7);
    Mvec.insert("s4", 4, 11.4);

    EXPECT_EQ(Mvec.back(), 11.4);

    EXPECT_EQ(Mvec.size(), 4);

    const auto& mvc = Mvec;

    EXPECT_EQ(mvc.back(), 11.4);

    EXPECT_EQ(mvc.size(), 4);

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

    EXPECT_EQ(mvc[0], 3.2 + 1.0);
    EXPECT_EQ(mvc[1], 4.3 + 1.0);
    EXPECT_EQ(mvc[2], 9.7 + 1.0);

    Mvec.apply(accum);
    EXPECT_DOUBLE_EQ(sum, sum1 + 4.0);
}
