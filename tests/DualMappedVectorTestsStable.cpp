/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "DualMappedVector.hpp"
#include "MapTraits.hpp"

#include "gtest/gtest.h"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using gmlc::containers::DualMappedVector;

/** test basic operations */
TEST(dual_mapped_vector_tests_stable, definition_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_double;
    const DualMappedVector<
        std::string,
        std::string,
        double,
        reference_stability::stable>
        mapped_string;
    EXPECT_EQ(mapped_double.size(), 0U);
    EXPECT_EQ(mapped_string.size(), 0U);
    const DualMappedVector<
        std::vector<std::string>,
        double,
        std::string,
        reference_stability::stable>
        mapped_vector;

    // test move and assignment operators
    decltype(mapped_double) target_vector;
    target_vector = std::move(mapped_double);

    decltype(target_vector) target_vector_copy;
    target_vector_copy = target_vector;
}

TEST(dual_mapped_vector_tests_stable, insertion_tests)
{
    DualMappedVector<
        std::vector<double>,
        std::string,
        int64_t,
        reference_stability::stable>
        mapped_vector;
    mapped_vector.insert("el1", 41, 3, 1.7);
    EXPECT_EQ(mapped_vector.size(), 1U);
    mapped_vector.insert("a2", 27, std::vector<double>(45));
    EXPECT_EQ(mapped_vector.size(), 2U);
    auto& first_vector = mapped_vector[0];
    EXPECT_EQ(first_vector.size(), 3U);
    EXPECT_EQ(first_vector[0], 1.7);
    EXPECT_EQ(first_vector[2], 1.7);

    auto& second_vector = mapped_vector[1];
    EXPECT_EQ(second_vector.size(), 45U);

    auto first_string_search = mapped_vector.find("el1");
    EXPECT_EQ(first_string_search->size(), 3U);

    auto second_string_search = mapped_vector.find("a2");
    EXPECT_EQ(second_string_search->size(), 45U);

    auto integer_search = mapped_vector.find(41);
    EXPECT_EQ(integer_search->size(), 3U);
}

TEST(dual_mapped_vector_tests_stable, insertion_tests2)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_vector;

    auto result = mapped_vector.insert("el1", 0, 1.7);
    EXPECT_TRUE(result);
    result = mapped_vector.insert("el2", 1, 3.4);
    EXPECT_TRUE(result);
    result = mapped_vector.insert("el2", 1, 22.22);
    EXPECT_FALSE(result);

    result = mapped_vector.insert("el3", ::no_search, 5.1);
    EXPECT_TRUE(result);

    result = mapped_vector.insert("el3", ::no_search, 9.8);
    EXPECT_FALSE(result);

    result = mapped_vector.insert(::no_search, 3, 9.9);
    EXPECT_TRUE(result);

    result = mapped_vector.insert(::no_search, 3, 14.7);
    EXPECT_FALSE(result);

    result = mapped_vector.insert(::no_search, ::no_search, 99.9);
    EXPECT_TRUE(result);
    EXPECT_EQ(mapped_vector[*result], 99.9);
}

TEST(dual_mapped_vector_tests_stable, assign_tests2)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_vector;

    mapped_vector.insert_or_assign("el1", 0, 1.7);

    auto inserted_location = mapped_vector.insert_or_assign("el2", 1, 3.4);

    auto updated_location = mapped_vector.insert_or_assign("el2", 1, 22.22);
    EXPECT_EQ(updated_location, inserted_location);

    updated_location = mapped_vector.insert_or_assign("el2", 4, 22.22);
    EXPECT_EQ(updated_location, inserted_location);

    updated_location = mapped_vector.insert_or_assign("el3", ::no_search, 5.1);

    auto second_updated_location =
        mapped_vector.insert_or_assign("el3", ::no_search, 9.8);
    EXPECT_EQ(updated_location, second_updated_location);
    EXPECT_EQ(mapped_vector[second_updated_location], 9.8);

    auto integer_location = mapped_vector.insert_or_assign(::no_search, 3, 9.9);
    EXPECT_GT(integer_location, second_updated_location);

    auto second_integer_location =
        mapped_vector.insert_or_assign(::no_search, 3, 14.7);
    EXPECT_EQ(second_integer_location, integer_location);
    EXPECT_EQ(mapped_vector[second_integer_location], 14.7);
}

TEST(dual_mapped_vector_tests_stable, dual_key_collision_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_vector;

    ASSERT_TRUE(mapped_vector.insert("el1", 1, 1.0));
    ASSERT_TRUE(mapped_vector.insert("el2", 2, 2.0));

    EXPECT_FALSE(mapped_vector.insert("el1", 3, 11.0));
    EXPECT_FALSE(mapped_vector.insert("el3", 2, 22.0));
    EXPECT_EQ(mapped_vector.size(), 2U);
    EXPECT_DOUBLE_EQ(*mapped_vector.find("el1"), 1.0);
    EXPECT_DOUBLE_EQ(*mapped_vector.find(2), 2.0);

    auto updated = mapped_vector.insert_or_assign("el1", 3, 5.0);
    EXPECT_EQ(updated, 0U);
    EXPECT_EQ(mapped_vector.find(1), mapped_vector.end());
    EXPECT_NE(mapped_vector.find(3), mapped_vector.end());
    EXPECT_DOUBLE_EQ(*mapped_vector.find("el1"), 5.0);

    EXPECT_THROW(
        mapped_vector.insert_or_assign("el1", 2, 6.0), std::invalid_argument);
    EXPECT_DOUBLE_EQ(*mapped_vector.find("el1"), 5.0);
    EXPECT_DOUBLE_EQ(*mapped_vector.find("el2"), 2.0);
}

TEST(dual_mapped_vector_tests_stable, additional_searchTerm_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_vector;

    mapped_vector.insert("s1", 64, 3.2);
    mapped_vector.insert("s2", 63, 4.3);
    mapped_vector.insert("s3", 47, 9.7);
    mapped_vector.insert("s4", 92, 11.4);

    EXPECT_EQ(mapped_vector.size(), 4U);

    mapped_vector.addSearchTerm("s5", "s1");
    EXPECT_EQ(mapped_vector.size(), 4U);

    EXPECT_EQ(*(mapped_vector.find("s5")), 3.2);

    mapped_vector.addSearchTerm(93, 47);
    EXPECT_EQ(*(mapped_vector.find(93)), 9.7);

    mapped_vector.addSearchTerm(143, "s3");
    EXPECT_EQ(*(mapped_vector.find(143)), 9.7);

    mapped_vector.addSearchTerm("extra", 63);
    EXPECT_EQ(*(mapped_vector.find("extra")), 4.3);

    mapped_vector.addSearchTermForIndex("astring", 3);
    EXPECT_EQ(*(mapped_vector.find("astring")), 11.4);

    auto result = mapped_vector.addSearchTermForIndex(99, 2);
    EXPECT_EQ(*(mapped_vector.find(99)), 9.7);
    EXPECT_TRUE(result);

    // check for appropriate return values
    EXPECT_TRUE(!mapped_vector.addSearchTerm("missing", "none"));
    EXPECT_TRUE(!mapped_vector.addSearchTerm(1241, 98));
    EXPECT_TRUE(!mapped_vector.addSearchTerm("missing", 98));
    EXPECT_TRUE(!mapped_vector.addSearchTerm(1241, "none"));
}

TEST(dual_mapped_vector_tests_stable, transform_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_vector;

    mapped_vector.insert("s1", 64, 3.2);
    mapped_vector.insert("s2", 63, 4.3);
    mapped_vector.insert("s3", 47, 9.7);
    mapped_vector.insert("s4", 92, 11.4);

    EXPECT_EQ(mapped_vector.size(), 4U);

    mapped_vector.transform([](double val) { return val + 1; });

    EXPECT_EQ(mapped_vector[0], 3.2 + 1.0);
    EXPECT_EQ(mapped_vector[1], 4.3 + 1.0);
    EXPECT_EQ(mapped_vector[2], 9.7 + 1.0);

    double sum = 0.0;
    for (const auto& element : mapped_vector) {
        sum += element;
    }
    EXPECT_EQ(sum, 4.2 + 5.3 + 10.7 + 12.4);
}

TEST(dual_mapped_vector_tests_stable, modify_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_vector;

    mapped_vector.insert("s1", 64, 3.2);
    mapped_vector.insert("s2", 63, 4.3);
    mapped_vector.insert("s3", 47, 9.7);
    mapped_vector.insert("s4", 92, 11.4);

    EXPECT_EQ(mapped_vector.size(), 4U);

    mapped_vector.modify([](double& val) { ++val; });

    EXPECT_EQ(mapped_vector[0], 3.2 + 1.0);
    EXPECT_EQ(mapped_vector[1], 4.3 + 1.0);
    EXPECT_EQ(mapped_vector[2], 9.7 + 1.0);
}

TEST(dual_mapped_vector_tests_stable, remove_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_vector;

    mapped_vector.insert("s1", 64, 3.2);
    mapped_vector.insert("s2", 63, 4.3);
    mapped_vector.insert("s3", 47, 9.7);
    mapped_vector.insert("s4", 92, 11.4);

    mapped_vector.addSearchTerm("s5", 64);
    mapped_vector.addSearchTermForIndex(107, 2);
    EXPECT_EQ(mapped_vector.size(), 4U);

    mapped_vector.removeIndex(1);

    EXPECT_TRUE(mapped_vector.find("s2") == mapped_vector.end());
    EXPECT_EQ(mapped_vector[3], 11.4);
    EXPECT_EQ(*mapped_vector.find("s4"), 11.4);
    EXPECT_EQ(*mapped_vector.find("s5"), 3.2);
    mapped_vector.remove("s4");
    EXPECT_EQ(mapped_vector.size(), 3U);
    EXPECT_TRUE(mapped_vector.find("s4") == mapped_vector.end());
    EXPECT_EQ(mapped_vector[2], 9.7);
    EXPECT_EQ(*mapped_vector.find(107), 9.7);

    auto moved_vector = std::move(mapped_vector);
    EXPECT_EQ(moved_vector.size(), 3U);

    auto copied_vector = moved_vector;
    EXPECT_EQ(moved_vector.size(), 3U);
    EXPECT_EQ(copied_vector.size(), 3U);

    copied_vector.remove(47);
    EXPECT_EQ(moved_vector.size(), 3U);
    EXPECT_EQ(copied_vector.size(), 2U);
    copied_vector.clear();
    EXPECT_EQ(moved_vector.size(), 3U);
    EXPECT_EQ(copied_vector.size(), 0U);
}

TEST(dual_mapped_vector_tests_stable, const_find_tests)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_vector;

    mapped_vector.insert("s1", 64, 3.2);
    mapped_vector.insert("s2", 63, 4.3);
    mapped_vector.insert("s3", 47, 9.7);
    mapped_vector.insert("s4", 92, 11.4);

    const auto& mapped_vector_const = mapped_vector;
    auto string_result = mapped_vector.find("s1");
    auto string_result_const = mapped_vector_const.find("s1");
    EXPECT_EQ(string_result, string_result_const);
    EXPECT_NE(string_result, mapped_vector.end());
    EXPECT_EQ(*string_result, 3.2);
    EXPECT_EQ(*string_result_const, 3.2);

    auto missing_string_result = mapped_vector.find("s7");
    auto missing_string_result_const = mapped_vector_const.find("s7");
    EXPECT_EQ(missing_string_result, missing_string_result_const);
    EXPECT_EQ(missing_string_result, mapped_vector.end());
    EXPECT_EQ(missing_string_result_const, mapped_vector.end());
    EXPECT_TRUE(missing_string_result == mapped_vector_const.end());
    EXPECT_TRUE(missing_string_result_const == mapped_vector_const.end());
    EXPECT_TRUE(mapped_vector_const.end() == missing_string_result);
    EXPECT_TRUE(mapped_vector_const.end() == missing_string_result_const);

    string_result = mapped_vector.find(63);
    string_result_const = mapped_vector_const.find(63);
    EXPECT_EQ(string_result, string_result_const);
    EXPECT_NE(string_result, mapped_vector.end());
    EXPECT_EQ(*string_result, 4.3);
    EXPECT_EQ(*string_result_const, 4.3);

    missing_string_result = mapped_vector.find(99);
    missing_string_result_const = mapped_vector_const.find(99);
    EXPECT_EQ(missing_string_result, missing_string_result_const);
    EXPECT_EQ(missing_string_result, mapped_vector.end());
    EXPECT_EQ(missing_string_result_const, mapped_vector.end());
    EXPECT_TRUE(missing_string_result == mapped_vector_const.end());
    EXPECT_TRUE(missing_string_result_const == mapped_vector_const.end());
    EXPECT_TRUE(mapped_vector_const.end() == missing_string_result);
    EXPECT_TRUE(mapped_vector_const.end() == missing_string_result_const);
}

TEST(dual_mapped_vector_tests_stable, empty_find)
{
    DualMappedVector<double, std::string, int64_t, reference_stability::stable>
        mapped_vector;
    auto result = mapped_vector.find("string1");
    EXPECT_EQ(result, mapped_vector.end());
}
