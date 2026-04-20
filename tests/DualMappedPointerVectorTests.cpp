/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "DualMappedPointerVector.hpp"

#include "gtest/gtest.h"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using gmlc::containers::DualMappedPointerVector;

/** test basic operations */
TEST(dual_mapped_pointer_vector_tests, definition_tests)
{
    DualMappedPointerVector<double, std::string, int64_t> mapped_double;
    const DualMappedPointerVector<std::string, std::string, double>
        mapped_string;
    EXPECT_EQ(mapped_double.size(), 0U);
    EXPECT_EQ(mapped_string.size(), 0U);
    const DualMappedPointerVector<std::vector<std::string>,
                                  double,
                                  std::string>
        mapped_vector;

    // test move operators
    decltype(mapped_double) target_vector;
    target_vector = std::move(mapped_double);

    const decltype(target_vector) target_vector_copy(std::move(target_vector));
}

TEST(dual_mapped_pointer_vector_tests, insertion_tests)
{
    DualMappedPointerVector<std::vector<double>, std::string, int64_t>
        mapped_vector;
    mapped_vector.insert("el1", 41, 3, 1.7);
    EXPECT_EQ(mapped_vector.size(), 1);
    mapped_vector.insert("a2", 27, std::vector<double>(45));
    EXPECT_EQ(mapped_vector.size(), 2);
    auto* first_vector = mapped_vector[0];
    EXPECT_EQ(first_vector->size(), 3);
    EXPECT_EQ((*first_vector)[0], 1.7);
    EXPECT_EQ((*first_vector)[2], 1.7);

    auto* second_vector = mapped_vector[1];
    EXPECT_EQ(second_vector->size(), 45);

    auto* first_string_search = mapped_vector.find("el1");
    EXPECT_EQ(first_string_search->size(), 3);

    auto* second_string_search = mapped_vector.find("a2");
    EXPECT_EQ(second_string_search->size(), 45);

    auto* integer_search = mapped_vector.find(41);
    EXPECT_EQ(integer_search->size(), 3);
}

TEST(dual_mapped_pointer_vector_tests, iterator_tests)
{
    DualMappedPointerVector<double, std::string, int64_t> mapped_vector;

    mapped_vector.insert("s1", 64, 3.2);
    mapped_vector.insert("s2", 63, 4.3);
    mapped_vector.insert("s3", 47, 9.7);
    mapped_vector.insert("s4", 92, 11.4);

    EXPECT_EQ(mapped_vector.size(), 4);

    mapped_vector.apply([](double* val) { *val = *val + 1; });

    EXPECT_EQ(*mapped_vector[0], 3.2 + 1.0);
    EXPECT_EQ(*mapped_vector[1], 4.3 + 1.0);
    EXPECT_EQ(*mapped_vector[2], 9.7 + 1.0);
}

TEST(dual_mapped_pointer_vector_tests, at_tests)
{
    DualMappedPointerVector<double, std::string, int64_t> mapped_vector;

    ASSERT_TRUE(mapped_vector.insert("s1", 64, 3.2));
    ASSERT_TRUE(mapped_vector.insert("s2", 63, 4.3));

    auto* value = mapped_vector.at(1);
    ASSERT_NE(value, nullptr);
    EXPECT_DOUBLE_EQ(*value, 4.3);

    *mapped_vector.at(0) = 7.1;
    EXPECT_DOUBLE_EQ(*mapped_vector.find("s1"), 7.1);

    EXPECT_THROW(static_cast<void>(mapped_vector.at(5)), std::out_of_range);
}

TEST(dual_mapped_pointer_vector_tests, remove_tests)
{
    DualMappedPointerVector<double, std::string, int64_t> mapped_vector;

    mapped_vector.insert("s1", 64, 3.2);
    mapped_vector.insert("s2", 63, 4.3);
    mapped_vector.insert("s3", 47, 9.7);
    mapped_vector.insert("s4", 92, 11.4);

    EXPECT_EQ(mapped_vector.size(), 4);

    mapped_vector.removeIndex(1);

    EXPECT_EQ(mapped_vector.size(), 3);
    EXPECT_TRUE(mapped_vector.find("s2") == nullptr);
    EXPECT_TRUE(mapped_vector.find(63) == nullptr);
    EXPECT_EQ(*mapped_vector[1], 9.7);
    EXPECT_EQ(*mapped_vector.find("s4"), 11.4);

    mapped_vector.remove("s1");
    EXPECT_EQ(mapped_vector.size(), 2);
    EXPECT_EQ(*mapped_vector.find("s4"), 11.4);
    EXPECT_EQ(*mapped_vector[0], 9.7);

    auto moved_vector = std::move(mapped_vector);
    EXPECT_EQ(moved_vector.size(), 2);

    auto second_moved_vector = std::move(moved_vector);
    EXPECT_EQ(second_moved_vector.size(), 2);

    second_moved_vector.remove(92);
    EXPECT_EQ(second_moved_vector.size(), 1);
    second_moved_vector.clear();
    EXPECT_EQ(second_moved_vector.size(), 0);
}

TEST(dual_mapped_pointer_vector_tests, dual_key_collision_tests)
{
    DualMappedPointerVector<double, std::string, int64_t> mapped_vector;

    ASSERT_TRUE(mapped_vector.insert("el1", 1, 1.0));
    ASSERT_TRUE(mapped_vector.insert("el2", 2, 2.0));

    EXPECT_FALSE(mapped_vector.insert("el1", 3, 11.0));
    EXPECT_FALSE(mapped_vector.insert("el3", 2, 22.0));
    EXPECT_EQ(mapped_vector.size(), 2U);
    ASSERT_NE(mapped_vector.find("el1"), nullptr);
    ASSERT_NE(mapped_vector.find(2), nullptr);
    EXPECT_DOUBLE_EQ(*mapped_vector.find("el1"), 1.0);
    EXPECT_DOUBLE_EQ(*mapped_vector.find(2), 2.0);

    auto updated = mapped_vector.insert_or_assign("el1", 3, 5.0);
    EXPECT_EQ(updated, 0U);
    EXPECT_EQ(mapped_vector.find(1), nullptr);
    ASSERT_NE(mapped_vector.find(3), nullptr);
    EXPECT_DOUBLE_EQ(*mapped_vector.find("el1"), 5.0);

    EXPECT_THROW(
        mapped_vector.insert_or_assign("el1", 2, 6.0), std::invalid_argument);
    EXPECT_DOUBLE_EQ(*mapped_vector.find("el1"), 5.0);
    EXPECT_DOUBLE_EQ(*mapped_vector.find("el2"), 2.0);
}

TEST(dual_mapped_pointer_vector_tests, remove_default_valued_keys)
{
    DualMappedPointerVector<double, int, int64_t> mapped_vector;

    ASSERT_TRUE(mapped_vector.insert(0, 100, 1.0));
    ASSERT_TRUE(mapped_vector.insert(5, static_cast<int64_t>(0), 2.0));

    mapped_vector.remove(5);

    EXPECT_EQ(mapped_vector.size(), 1U);
    ASSERT_NE(mapped_vector.find(0), nullptr);
    EXPECT_DOUBLE_EQ(*mapped_vector.find(0), 1.0);
    EXPECT_EQ(mapped_vector.find(static_cast<int64_t>(0)), nullptr);

    mapped_vector.remove(static_cast<int64_t>(100));
    EXPECT_EQ(mapped_vector.size(), 0U);
    EXPECT_EQ(mapped_vector.find(0), nullptr);
}
