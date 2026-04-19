/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "StringMappedVector.hpp"
#include "MapTraits.hpp"

#include "gtest/gtest.h"
#include <string>
#include <utility>
#include <vector>

using gmlc::containers::StringMappedVector;

/** test basic operations */
TEST(string_mapped_vector, definition_tests)
{
    StringMappedVector<double> mapped_double;
    const StringMappedVector<std::string> mapped_string;
    EXPECT_EQ(mapped_double.size(), 0U);
    EXPECT_EQ(mapped_string.size(), 0U);
    const StringMappedVector<std::vector<std::string>> mapped_vector;

    // test move and assignment operators
    [[maybe_unused]] const auto mapped_vector_copy = mapped_vector;
    decltype(mapped_double) target_vector;
    target_vector = std::move(mapped_double);

    decltype(target_vector) target_vector_copy;
    target_vector_copy = target_vector;
}

TEST(string_mapped_vector, insertion_tests)
{
    StringMappedVector<std::vector<double>> mapped_vector;
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

    auto loc = mapped_vector.insert("a2", std::vector<double>(14));
    EXPECT_FALSE(loc);

    auto missing_search = mapped_vector.find("not_available");
    EXPECT_EQ(missing_search, mapped_vector.end());

    const auto& mapped_vector_const = mapped_vector;
    auto second_search_const = mapped_vector_const.find("a2");
    EXPECT_EQ(second_search_const->size(), 45);

    auto missing_search_const = mapped_vector_const.find("not_available");
    EXPECT_EQ(missing_search_const, mapped_vector_const.end());

    const auto& second_vector_const = mapped_vector_const[1];
    EXPECT_EQ(second_vector_const.size(), 45);
    EXPECT_EQ(mapped_vector_const.back().size(), 45);
}

TEST(string_mapped_vector, insert_or_assign)
{
    StringMappedVector<std::vector<double>> mapped_vector;
    mapped_vector.insert("el1", 3, 1.7);
    EXPECT_EQ(mapped_vector.size(), 1U);
    auto loc_a2 = mapped_vector.insert("a2", std::vector<double>(45));
    EXPECT_TRUE(loc_a2);
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

    auto loc = mapped_vector.insert("a2", std::vector<double>(14));
    EXPECT_FALSE(loc);

    auto loc_a2_insert_or_assign =
        mapped_vector.insert_or_assign("a2", std::vector<double>(14));
    EXPECT_EQ(loc_a2_insert_or_assign, *loc_a2);
    auto updated_search = mapped_vector.find("a2");
    EXPECT_EQ(updated_search->size(), 14);

    EXPECT_EQ(mapped_vector.back().size(), 14);

    auto loc_a3_insert_or_assign =
        mapped_vector.insert_or_assign("a3", std::vector<double>(16));

    EXPECT_EQ(mapped_vector.back().size(), 16);
    EXPECT_EQ(mapped_vector[loc_a3_insert_or_assign].size(), 16);
    EXPECT_EQ(loc_a3_insert_or_assign, mapped_vector.size() - 1);
}

TEST(string_mapped_vector, insertion_tests_nomap)
{
    StringMappedVector<std::vector<double>> mapped_vector;
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

    auto loc = mapped_vector.insert(::no_search, std::vector<double>(22));
    EXPECT_TRUE(loc);
    auto& inserted_vector = mapped_vector[*loc];
    EXPECT_EQ(inserted_vector.size(), 22);
}

TEST(string_mapped_vector, apply_tests)
{
    StringMappedVector<double> mapped_vector;

    mapped_vector.insert("s1", 3.2);
    mapped_vector.insert("s2", 4.3);
    mapped_vector.insert("s3", 9.7);
    mapped_vector.insert("s4", 11.4);

    EXPECT_EQ(mapped_vector.size(), 4);

    double sum = 0;
    auto accum = [&sum](double val) { sum += val; };

    mapped_vector.apply(accum);
    EXPECT_DOUBLE_EQ(sum, 3.2 + 4.3 + 9.7 + 11.4);
    const double sum1 = sum;
    sum = 0.0;
    mapped_vector.transform([](double val) { return val + 1; });

    EXPECT_EQ(mapped_vector[0], 3.2 + 1.0);
    EXPECT_EQ(mapped_vector[1], 4.3 + 1.0);
    EXPECT_EQ(mapped_vector[2], 9.7 + 1.0);

    mapped_vector.apply(accum);
    EXPECT_DOUBLE_EQ(sum, sum1 + 4.0);
}

TEST(mapped_vector_tests, modify_tests)
{
    StringMappedVector<double> mapped_vector;

    mapped_vector.insert("s1", 3.2);
    mapped_vector.insert("s2", 4.3);
    mapped_vector.insert("s3", 9.7);
    mapped_vector.insert("s4", 11.4);

    EXPECT_EQ(mapped_vector.size(), 4);

    mapped_vector.modify([](double& val) { ++val; });

    EXPECT_EQ(mapped_vector[0], 3.2 + 1.0);
    EXPECT_EQ(mapped_vector[1], 4.3 + 1.0);
    EXPECT_EQ(mapped_vector[2], 9.7 + 1.0);
}

TEST(string_mapped_vector, remove_tests)
{
    StringMappedVector<double> mapped_vector;

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

TEST(string_mapped_vector, empty_find)
{
    StringMappedVector<double> mapped_vector;
    auto result = mapped_vector.find("string1");
    EXPECT_EQ(result, mapped_vector.end());
}
