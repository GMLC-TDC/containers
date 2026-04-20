/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "MapTraits.hpp"
#include "MappedPointerVector.hpp"

#include "gtest/gtest.h"
#include <string>
#include <utility>
#include <vector>

using gmlc::containers::MappedPointerVector;

/** test basic operations */
TEST(mapped_pointer_vector, definition)
{
    MappedPointerVector<double> mapped_double;
    const MappedPointerVector<std::string> mapped_string;
    EXPECT_EQ(mapped_double.size(), 0U);
    EXPECT_EQ(mapped_string.size(), 0U);
    const MappedPointerVector<std::vector<std::string>, double> mapped_vector;

    // test move operators
    decltype(mapped_double) target_vector;
    target_vector = std::move(mapped_double);

    const decltype(target_vector) target_vector_copy(std::move(target_vector));
}

TEST(mapped_pointer_vector, insertion)
{
    MappedPointerVector<std::vector<double>> mapped_vector;
    mapped_vector.insert("el1", 3, 1.7);
    EXPECT_EQ(mapped_vector.size(), 1);
    mapped_vector.insert("a2", std::vector<double>(45));
    EXPECT_EQ(mapped_vector.size(), 2);
    auto* first_vector = mapped_vector[0];
    EXPECT_EQ(first_vector->size(), 3);
    EXPECT_EQ((*first_vector)[0], 1.7);
    EXPECT_EQ((*first_vector)[2], 1.7);

    auto* second_vector = mapped_vector[1];
    EXPECT_EQ(second_vector->size(), 45);

    auto* first_search = mapped_vector.find("el1");
    EXPECT_EQ(first_search->size(), 3);

    auto* second_search = mapped_vector.find("a2");
    EXPECT_EQ(second_search->size(), 45);
}

TEST(mapped_pointer_vector, insertion_no_search)
{
    MappedPointerVector<std::vector<double>> mapped_vector;
    mapped_vector.insert("el1", 3, 1.7);
    EXPECT_EQ(mapped_vector.size(), 1);
    auto result = mapped_vector.insert(::no_search_type(), std::vector<double>(45));
    EXPECT_EQ(mapped_vector.size(), 2);
    EXPECT_TRUE(result);
    auto* first_vector = mapped_vector[0];
    EXPECT_EQ(first_vector->size(), 3);
    EXPECT_EQ((*first_vector)[0], 1.7);
    EXPECT_EQ((*first_vector)[2], 1.7);

    auto* second_vector = mapped_vector[1];
    EXPECT_EQ(second_vector->size(), 45);

    auto* first_search = mapped_vector.find("el1");
    EXPECT_EQ(first_search->size(), 3);

    auto* second_search = mapped_vector.find("a2");
    EXPECT_EQ(second_search, nullptr);

    EXPECT_TRUE(mapped_vector.addSearchTermForIndex("a2", *result));
    second_search = mapped_vector.find("a2");
    EXPECT_NE(second_search, nullptr);
}

TEST(mapped_pointer_vector, iterator)
{
    MappedPointerVector<double> mapped_vector;

    mapped_vector.insert("s1", 3.2);
    mapped_vector.insert("s2", 4.3);
    mapped_vector.insert("s3", 9.7);
    mapped_vector.insert("s4", 11.4);

    EXPECT_EQ(mapped_vector.size(), 4);

    mapped_vector.apply([](double* val) { *val = *val + 1; });

    EXPECT_EQ(*mapped_vector[0], 3.2 + 1.0);
    EXPECT_EQ(*mapped_vector[1], 4.3 + 1.0);
    EXPECT_EQ(*mapped_vector[2], 9.7 + 1.0);
}

TEST(mapped_pointer_vector, remove)
{
    MappedPointerVector<double> mapped_vector;

    mapped_vector.insert("s1", 3.2);
    mapped_vector.insert("s2", 4.3);
    mapped_vector.insert("s3", 9.7);
    mapped_vector.insert("s4", 11.4);

    EXPECT_EQ(mapped_vector.size(), 4);

    mapped_vector.removeIndex(1);

    EXPECT_EQ(mapped_vector.size(), 3);
    EXPECT_TRUE(mapped_vector.find("s2") == nullptr);
    EXPECT_EQ(*mapped_vector[1], 9.7);
    EXPECT_EQ(*mapped_vector.find("s4"), 11.4);

    mapped_vector.remove("s1");
    EXPECT_EQ(mapped_vector.size(), 2);
    EXPECT_EQ(*mapped_vector.find("s4"), 11.4);
    EXPECT_EQ(*mapped_vector[0], 9.7);

    auto moved_vector = std::move(mapped_vector);
    EXPECT_EQ(moved_vector.size(), 2);

    moved_vector.clear();
    EXPECT_EQ(moved_vector.size(), 0);
}

TEST(mapped_pointer_vector, remove_alias_and_default_key)
{
    MappedPointerVector<double, int> mapped_vector;

    ASSERT_TRUE(mapped_vector.insert(0, 3.2));
    ASSERT_TRUE(mapped_vector.insert(5, 4.3));
    ASSERT_TRUE(mapped_vector.addSearchTermForIndex(7, 1));

    mapped_vector.remove(5);

    EXPECT_EQ(mapped_vector.size(), 1);
    EXPECT_NE(mapped_vector.find(0), nullptr);
    EXPECT_DOUBLE_EQ(*mapped_vector.find(0), 3.2);
    EXPECT_EQ(mapped_vector.find(5), nullptr);
    EXPECT_EQ(mapped_vector.find(7), nullptr);
}
