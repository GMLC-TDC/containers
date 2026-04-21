/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "MapTraits.hpp"
#include "MappedVector.hpp"

#include "gtest/gtest.h"
#include <string>
#include <utility>
#include <vector>

using gmlc::containers::MappedVector;

/** test basic operations */
TEST(stable_mapped_vector_tests, definition_tests)
{
    MappedVector<double, std::string, reference_stability::stable>
        mapped_double;
    const MappedVector<std::string, std::string, reference_stability::stable>
        mapped_string;
    EXPECT_EQ(mapped_double.size(), 0U);
    EXPECT_EQ(mapped_string.size(), 0U);
    const MappedVector<
        std::vector<std::string>,
        double,
        reference_stability::stable>
        mapped_vector;

    // test move and assignment operators
    decltype(mapped_double) target_vector;
    target_vector = std::move(mapped_double);

    decltype(target_vector) target_vector_copy;
    target_vector_copy = target_vector;
}

TEST(stable_mapped_vector_tests, insertion_tests)
{
    MappedVector<std::vector<double>, std::string, reference_stability::stable>
        mapped_vector;
    mapped_vector.insert("el1", 3, 1.7);
    EXPECT_EQ(mapped_vector.size(), 1U);
    mapped_vector.insert("a2", std::vector<double>(45));
    EXPECT_EQ(mapped_vector.size(), 2U);
    auto& first_vector = mapped_vector[0];
    EXPECT_EQ(first_vector.size(), 3U);
    EXPECT_EQ(first_vector[0], 1.7);
    EXPECT_EQ(first_vector[2], 1.7);

    auto& second_vector = mapped_vector[1];
    EXPECT_EQ(second_vector.size(), 45);

    auto first_search = mapped_vector.find("el1");
    EXPECT_EQ(first_search->size(), 3);

    auto second_search = mapped_vector.find("a2");
    EXPECT_EQ(second_search->size(), 45);
}

TEST(stable_mapped_vector_tests, iterator_tests)
{
    MappedVector<double, std::string, reference_stability::stable>
        mapped_vector;

    mapped_vector.insert("s1", 3.2);
    mapped_vector.insert("s2", 4.3);
    mapped_vector.insert("s3", 9.7);
    mapped_vector.insert("s4", 11.4);

    EXPECT_EQ(mapped_vector.size(), 4);

    mapped_vector.transform([](double val) { return val + 1; });

    EXPECT_EQ(mapped_vector[0], 3.2 + 1.0);
    EXPECT_EQ(mapped_vector[1], 4.3 + 1.0);
    EXPECT_EQ(mapped_vector[2], 9.7 + 1.0);
}

TEST(stable_mapped_vector_tests, remove_tests)
{
    MappedVector<double, std::string, reference_stability::stable>
        mapped_vector;

    mapped_vector.insert("s1", 3.2);
    mapped_vector.insert("s2", 4.3);
    mapped_vector.insert("s3", 9.7);
    mapped_vector.insert("s4", 11.4);

    EXPECT_EQ(mapped_vector.size(), 4);

    mapped_vector.removeIndex(1);

    EXPECT_TRUE(mapped_vector.find("s2") == mapped_vector.end());
    EXPECT_EQ(mapped_vector[1], 4.3);
    EXPECT_EQ(*mapped_vector.find("s4"), 11.4);

    mapped_vector.removeIndex(3);
    EXPECT_TRUE(mapped_vector.find("s4") == mapped_vector.end());

    mapped_vector.remove("s1");
    EXPECT_EQ(*mapped_vector.find("s3"), 9.7);
    EXPECT_EQ(mapped_vector[0], 3.2);

    auto moved_vector = std::move(mapped_vector);
    EXPECT_EQ(moved_vector.size(), 3);

    auto copied_vector = moved_vector;
    EXPECT_EQ(moved_vector.size(), 3);
    EXPECT_EQ(copied_vector.size(), 3);

    copied_vector.clear();
    EXPECT_EQ(moved_vector.size(), 3);
    EXPECT_EQ(copied_vector.size(), 0);
}

TEST(mapped_vector_tests_stable, empty_find)
{
    MappedVector<double, std::string, reference_stability::stable>
        mapped_vector;
    auto result = mapped_vector.find("string1");
    EXPECT_EQ(result, mapped_vector.end());
}
