/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "StableBlockVector.hpp"

#include "gtest/gtest.h"
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using gmlc::containers::StableBlockVector;

TEST(stableBlockVectorTest, test_lookup)
{
    StableBlockVector<std::string, 2> stable_block_vector;
    EXPECT_TRUE(stable_block_vector.empty());
    stable_block_vector.push_back("bob");
    stable_block_vector.emplace_back("bob2");
    stable_block_vector.emplace_back("bob3");
    stable_block_vector.emplace_back("bob4");
    stable_block_vector.emplace_back("bob5");
    EXPECT_EQ(stable_block_vector.size(), 5);
    EXPECT_EQ(stable_block_vector[0], "bob");
    EXPECT_EQ(stable_block_vector[1], "bob2");
    stable_block_vector[1] = "bob2_mod";
    EXPECT_EQ(stable_block_vector[1], "bob2_mod");
    EXPECT_EQ(stable_block_vector[2], "bob3");
    EXPECT_EQ(stable_block_vector[3], "bob4");
    EXPECT_EQ(stable_block_vector[4], "bob5");

    EXPECT_EQ(stable_block_vector.at(0), "bob");
    EXPECT_EQ(stable_block_vector.at(1), "bob2_mod");
    stable_block_vector.at(1) = "bob2_mod2";
    EXPECT_EQ(stable_block_vector.at(1), "bob2_mod2");
    EXPECT_EQ(stable_block_vector.at(2), "bob3");
    EXPECT_EQ(stable_block_vector.at(3), "bob4");
    EXPECT_EQ(stable_block_vector.at(4), "bob5");
    EXPECT_FALSE(stable_block_vector.empty());
    EXPECT_THROW(stable_block_vector.at(10), std::out_of_range);
}

TEST(stableBlockVectorTest, test_const_lookup)
{
    StableBlockVector<std::string, 2> stable_block_vector;
    stable_block_vector.push_back("bob");
    stable_block_vector.emplace_back("bob2");
    stable_block_vector.emplace_back("bob3");
    stable_block_vector.emplace_back("bob4");
    stable_block_vector.push_back("bob5");
    const auto& stable_block_vector_const = stable_block_vector;
    EXPECT_EQ(stable_block_vector_const.size(), 5);
    EXPECT_EQ(stable_block_vector_const[0], "bob");
    EXPECT_EQ(stable_block_vector_const[1], "bob2");
    stable_block_vector[1] = "bob2_mod";
    EXPECT_EQ(stable_block_vector_const[1], "bob2_mod");
    EXPECT_EQ(stable_block_vector_const[2], "bob3");
    EXPECT_EQ(stable_block_vector_const[3], "bob4");
    EXPECT_EQ(stable_block_vector_const[4], "bob5");
    EXPECT_FALSE(stable_block_vector_const.empty());

    EXPECT_EQ(stable_block_vector_const.at(1), "bob2_mod");
    EXPECT_EQ(stable_block_vector_const.at(2), "bob3");
    EXPECT_EQ(stable_block_vector_const.at(3), "bob4");
    EXPECT_EQ(stable_block_vector_const.at(4), "bob5");

    EXPECT_THROW(stable_block_vector_const.at(10), std::out_of_range);
}

TEST(stableBlockVectorTest, test_iterator)
{
    StableBlockVector<std::string, 2> stable_block_vector;
    stable_block_vector.push_back("bob");
    stable_block_vector.emplace_back("bob2");
    stable_block_vector.emplace_back("bob3");
    stable_block_vector.emplace_back("bob4");
    stable_block_vector.emplace_back("bob5");
    EXPECT_EQ(stable_block_vector.size(), 5);
    auto iterator = stable_block_vector.begin();
    EXPECT_EQ(*iterator, "bob");
    ++iterator;
    EXPECT_EQ(*iterator, "bob2");
    iterator++;
    EXPECT_EQ(*iterator, "bob3");
    --iterator;
    EXPECT_EQ(*iterator, "bob2");
    iterator += 1;
    EXPECT_EQ(*iterator, "bob3");
    iterator += 2;
    EXPECT_EQ(*iterator, "bob5");
    iterator -= 1;
    EXPECT_EQ(*iterator--, "bob4");
    EXPECT_EQ(*iterator, "bob3");
    iterator -= 2;
    EXPECT_EQ(*iterator, "bob");
    int index = 0;
    while (iterator != stable_block_vector.end()) {
        ++index;
        ++iterator;
    }
    EXPECT_EQ(index, 5);
}

TEST(stableBlockVectorTest, test_iterator2)
{
    StableBlockVector<std::string, 2> stable_block_vector;
    stable_block_vector.push_back("bob");
    stable_block_vector.emplace_back("bob2");
    stable_block_vector.emplace_back("bob3");
    stable_block_vector.emplace_back("bob4");
    EXPECT_EQ(stable_block_vector.size(), 4);
    auto iterator = stable_block_vector.begin();
    EXPECT_EQ(*iterator, "bob");
    ++iterator;
    EXPECT_EQ(*iterator, "bob2");
    iterator++;
    EXPECT_EQ(*iterator, "bob3");
    --iterator;
    EXPECT_EQ(*iterator, "bob2");
    iterator += 2;
    EXPECT_EQ(*iterator--, "bob4");
    EXPECT_EQ(*iterator, "bob3");
    iterator -= 2;
    EXPECT_EQ(*iterator, "bob");
    int index = 0;
    while (iterator != stable_block_vector.end()) {
        ++index;
        ++iterator;
    }
    EXPECT_EQ(index, 4);
}

TEST(stableBlockVectorTest, test_const_iterator)
{
    StableBlockVector<std::string, 2> stable_block_vector;
    stable_block_vector.push_back("bob");
    stable_block_vector.emplace_back("bob2");
    stable_block_vector.emplace_back("bob3");
    stable_block_vector.emplace_back("bob4");
    stable_block_vector.emplace_back("bob5");

    const auto& stable_block_vector_const = stable_block_vector;
    EXPECT_EQ(stable_block_vector_const.size(), 5);
    auto iterator = stable_block_vector_const.begin();
    EXPECT_EQ(*iterator, "bob");
    ++iterator;
    EXPECT_EQ(*iterator, "bob2");
    iterator++;
    EXPECT_EQ(*iterator, "bob3");
    --iterator;
    EXPECT_EQ(*iterator, "bob2");
    iterator += 1;
    EXPECT_EQ(*iterator, "bob3");
    iterator += 2;
    EXPECT_EQ(*iterator, "bob5");
    iterator -= 1;
    EXPECT_EQ(*iterator--, "bob4");
    EXPECT_EQ(*iterator, "bob3");
    iterator -= 2;
    EXPECT_EQ(*iterator, "bob");
    int index = 0;
    while (iterator != stable_block_vector_const.end()) {
        ++index;
        ++iterator;
    }
    EXPECT_EQ(index, 5);
}

TEST(stableBlockVectorTest, test_const_iterator2)
{
    StableBlockVector<std::string, 2> stable_block_vector;
    stable_block_vector.push_back("bob");
    stable_block_vector.emplace_back("bob2");
    stable_block_vector.emplace_back("bob3");
    stable_block_vector.emplace_back("bob4");
    const auto& stable_block_vector_const = stable_block_vector;
    EXPECT_EQ(stable_block_vector_const.size(), 4);
    EXPECT_EQ(stable_block_vector_const.back(), "bob4");
    auto iterator = stable_block_vector_const.begin();
    EXPECT_EQ(*iterator, "bob");
    ++iterator;
    EXPECT_EQ(*iterator, "bob2");
    iterator++;
    EXPECT_EQ(*iterator, "bob3");
    --iterator;
    EXPECT_EQ(*iterator, "bob2");
    iterator += 2;
    EXPECT_EQ(*iterator--, "bob4");
    EXPECT_EQ(*iterator, "bob3");
    iterator -= 2;
    EXPECT_EQ(*iterator, "bob");
    int index = 0;
    while (iterator != stable_block_vector_const.end()) {
        ++index;
        ++iterator;
    }
    EXPECT_EQ(index, 4);
    EXPECT_FALSE(stable_block_vector_const.empty());
    EXPECT_FALSE(stable_block_vector.empty());
}

TEST(stableBlockVectorTest, test_simple)
{
    StableBlockVector<std::string, 2> stable_block_vector;
    const std::string bob7{"bob7"};
    stable_block_vector.push_back("bob");
    stable_block_vector.emplace_back("bob2");
    stable_block_vector.emplace_back("bob3");
    stable_block_vector.emplace_back("bob4");
    stable_block_vector.emplace_back("bob5");
    stable_block_vector.emplace_back("bob6");
    stable_block_vector.push_back(bob7);
    EXPECT_EQ(stable_block_vector.back(), "bob7");
    stable_block_vector.emplace_back("bob8");
    stable_block_vector.emplace_back("bob9");
    stable_block_vector.emplace_back("bob10");
    stable_block_vector.push_back("bob11");
    stable_block_vector.emplace_back("bob12");
    stable_block_vector.emplace_back("bob13");
    stable_block_vector.emplace_back("bob14");
    int index = 0;
    std::vector<std::string> actual;
    for (const auto& element : stable_block_vector) {
        actual.push_back(element);
        ++index;
    }
    EXPECT_EQ(stable_block_vector.size(), 14);
    EXPECT_EQ(index, 14);

    stable_block_vector.pop_back();
    stable_block_vector.pop_back();
    stable_block_vector.pop_back();
    stable_block_vector.pop_back();
    auto& stable_block_vector_alias = stable_block_vector;
    index = 0;
    for (const auto& element : stable_block_vector_alias) {
        (void)(element);
        ++index;
    }
    EXPECT_EQ(stable_block_vector.size(), 10);
    EXPECT_EQ(stable_block_vector_alias.size(), 10);
    EXPECT_EQ(index, 10);
}

TEST(stableBlockVectorTest, iterator_check)
{
    for (size_t size = 1; size < 120; ++size) {
        StableBlockVector<std::string, 4> stable_block_vector(size);
        const auto& stable_block_vector_copy = stable_block_vector;
        auto iterator = stable_block_vector.begin();
        size_t index = 0;
        while (iterator != stable_block_vector.end()) {
            *iterator = std::to_string(index);
            ++iterator;
            ++index;
        }
        EXPECT_EQ(index, size);
        for (index = 0; index < size; ++index) {
            const auto text = std::to_string(index);
            const auto& result = stable_block_vector[index];
            EXPECT_EQ(text, result);
            EXPECT_EQ(text, stable_block_vector_copy[index]);
        }
    }
}

TEST(stableBlockVectorTest, test_back)
{
    StableBlockVector<size_t, 4> stable_block_vector(100);
    const auto& stable_block_vector_copy = stable_block_vector;
    size_t index = 0;
    for (auto& value : stable_block_vector) {
        value = index++;
    }
    EXPECT_EQ(stable_block_vector_copy.back(), 99U);
    for (int reverse_index = 99; reverse_index >= 0; --reverse_index) {
        EXPECT_EQ(
            stable_block_vector.back(), static_cast<size_t>(reverse_index));
        EXPECT_EQ(
            stable_block_vector_copy.back(),
            static_cast<size_t>(reverse_index));
        EXPECT_EQ(*stable_block_vector_copy.begin(), 0U);

        stable_block_vector.pop_back();
    }

    EXPECT_NO_THROW(stable_block_vector.pop_back());
}

TEST(stableBlockVectorTest, constructor1)
{
    StableBlockVector<std::string, 7> stable_block_vector{
        15, "this is an exciting long string"};
    EXPECT_EQ(stable_block_vector.size(), 15);
    for (size_t index = 0; index < 15; ++index) {
        EXPECT_EQ(
            stable_block_vector[index], "this is an exciting long string");
    }
}

TEST(stableBlockVectorTest, constructor0)
{
    const StableBlockVector<std::string, 7> stable_block_vector{
        0, "this is an exciting long string"};
    EXPECT_TRUE(stable_block_vector.empty());
    EXPECT_EQ(stable_block_vector.size(), 0);
}

TEST(stableBlockVectorTest, copy_constructor)
{
    const StableBlockVector<std::string, 7> stable_block_vector{
        15, "this is an exciting long string"};
    const auto& stable_block_vector_copy = stable_block_vector;
    EXPECT_EQ(stable_block_vector_copy.size(), 15);
    for (size_t index = 0; index < 15; ++index) {
        EXPECT_EQ(
            stable_block_vector_copy[index], "this is an exciting long string");
    }

    const StableBlockVector<std::string, 4> stable_block_vector_source;
    const auto& stable_block_vector_default = stable_block_vector_source;
    EXPECT_TRUE(stable_block_vector_default.empty());
}

TEST(stableBlockVectorTest, copy_construct_empty)
{
    StableBlockVector<size_t, 4> stable_block_vector;
    EXPECT_TRUE(stable_block_vector.empty());
    stable_block_vector.clear();
    EXPECT_TRUE(stable_block_vector.empty());
    StableBlockVector<size_t, 4> stable_block_vector2(stable_block_vector);

    EXPECT_EQ(stable_block_vector2.size(), stable_block_vector.size());
    EXPECT_EQ(stable_block_vector2.size(), 0);
    stable_block_vector2.clear();
    EXPECT_EQ(stable_block_vector2.size(), stable_block_vector.size());
    EXPECT_EQ(stable_block_vector2.size(), 0);
}

TEST(stableBlockVectorTest, move_constructor)
{
    StableBlockVector<std::string, 7> stable_block_vector{
        15, "this is an exciting long string"};
    auto stable_block_vector2 = std::move(stable_block_vector);
    EXPECT_EQ(stable_block_vector2.size(), 15);
    for (size_t index = 0; index < 15; ++index) {
        EXPECT_EQ(
            stable_block_vector2[index], "this is an exciting long string");
    }
}

TEST(stableBlockVectorTest, move_assign)
{
    StableBlockVector<size_t, 4> stable_block_vector2(100);
    {
        StableBlockVector<size_t, 4> stable_block_vector(200);
        size_t index = 0;
        for (auto& value : stable_block_vector) {
            value = index++;
        }
        stable_block_vector2 = std::move(stable_block_vector);
    }
    EXPECT_EQ(stable_block_vector2.size(), 200);
    for (size_t index = 0; index < 200; ++index) {
        EXPECT_EQ(stable_block_vector2.front(), 0U);
        EXPECT_EQ(stable_block_vector2.back(), 199 - index);

        stable_block_vector2.pop_back();
    }
}

TEST(stableBlockVectorTest, assign_move)
{
    std::vector<std::string> vector1(200, std::string(200, 'a'));
    StableBlockVector<std::string, 5> stable_block_vector2(
        100, std::string(100, 'b'));

    stable_block_vector2.move_assign(vector1.begin(), vector1.end());
    EXPECT_EQ(stable_block_vector2.size(), 200);
    for (size_t index = 0; index < 200U; ++index) {
        EXPECT_EQ(stable_block_vector2.back(), std::string(200, 'a'));
        stable_block_vector2.pop_back();
    }
}

TEST(stableBlockVectorTest, assign_move_to_empty)
{
    std::vector<std::string> vector1(200, std::string(200, 'a'));
    StableBlockVector<std::string, 5> stable_block_vector2;

    stable_block_vector2.move_assign(vector1.begin(), vector1.end());
    EXPECT_EQ(stable_block_vector2.size(), 200);
    for (size_t index = 0; index < 200; ++index) {
        EXPECT_EQ(stable_block_vector2.back(), std::string(200, 'a'));
        stable_block_vector2.pop_back();
    }
}

TEST(stableBlockVectorTest, assign_move_to_bigger)
{
    std::vector<std::string> vector1(200, std::string(200, 'a'));
    StableBlockVector<std::string, 5> stable_block_vector2(
        500, std::string(100, 'b'));

    stable_block_vector2.move_assign(vector1.begin(), vector1.end());
    EXPECT_EQ(stable_block_vector2.size(), 200);
    for (size_t index = 0; index < 200; ++index) {
        EXPECT_EQ(stable_block_vector2.front(), std::string(200, 'a'));
        EXPECT_EQ(stable_block_vector2.back(), std::string(200, 'a'));

        stable_block_vector2.pop_back();
    }
}

TEST(stableBlockVectorTest, copy_assign)
{
    StableBlockVector<size_t, 4> stable_block_vector2(100);

    StableBlockVector<size_t, 4> stable_block_vector(200);
    size_t index = 0;
    for (auto& value : stable_block_vector) {
        value = index++;
    }
    stable_block_vector2 = stable_block_vector;

    EXPECT_EQ(stable_block_vector2.size(), 200);
    for (index = 0; index < 200; ++index) {
        EXPECT_EQ(stable_block_vector2.front(), 0U);
        EXPECT_EQ(
            stable_block_vector2.back(), stable_block_vector[199 - index]);

        stable_block_vector2.pop_back();
    }
}

TEST(stableBlockVectorTest, copy_assign_from_empty)
{
    StableBlockVector<size_t, 4> stable_block_vector2;

    StableBlockVector<size_t, 4> stable_block_vector(200);
    size_t index = 0;
    for (auto& value : stable_block_vector) {
        value = index++;
    }
    stable_block_vector2 = stable_block_vector;

    EXPECT_EQ(stable_block_vector2.size(), 200);
    for (index = 0; index < 200; ++index) {
        EXPECT_EQ(stable_block_vector2.front(), stable_block_vector[0]);
        EXPECT_EQ(
            stable_block_vector2.back(), stable_block_vector[199 - index]);

        stable_block_vector2.pop_back();
    }
}

TEST(stableBlockVectorTest, copy_assign_to_bigger)
{
    StableBlockVector<size_t, 4> stable_block_vector2(400, 5);

    StableBlockVector<size_t, 4> stable_block_vector(200);
    size_t index = 0;
    for (auto& value : stable_block_vector) {
        value = index++;
    }
    stable_block_vector2 = stable_block_vector;

    EXPECT_EQ(stable_block_vector2.size(), 200);
    for (index = 0; index < 200; ++index) {
        EXPECT_EQ(stable_block_vector2.front(), stable_block_vector[0]);
        EXPECT_EQ(
            stable_block_vector2.back(), stable_block_vector[199 - index]);

        stable_block_vector2.pop_back();
    }
}

TEST(stableBlockVectorTest, clear_and_fill)
{
    StableBlockVector<size_t, 4> stable_block_vector(200);
    EXPECT_EQ(stable_block_vector.size(), 200U);
    stable_block_vector.clear();
    EXPECT_EQ(stable_block_vector.size(), 0U);
    for (size_t index = 0; index < 400; ++index) {
        stable_block_vector.push_back(index);
    }
    EXPECT_EQ(stable_block_vector.size(), 400U);
}

TEST(stableBlockVectorTest, fill_large_back)
{
    StableBlockVector<size_t, 4> stable_block_vector(200, 999999999UL);
    for (int index = 0; index < 10000; ++index) {
        stable_block_vector.push_back(static_cast<size_t>(index));
    }

    EXPECT_EQ(stable_block_vector.size(), 10200);
    for (int index = 0; index < 10000; ++index) {
        stable_block_vector.pop_back();
    }
    EXPECT_EQ(stable_block_vector.size(), 200);
}

TEST(stableBlockVectorTest, iterators)
{
    StableBlockVector<size_t, 3> stable_block_vector;
    EXPECT_TRUE(stable_block_vector.begin() == stable_block_vector.end());
    const auto& stable_block_vector_const = stable_block_vector;
    EXPECT_TRUE(
        stable_block_vector_const.begin() == stable_block_vector_const.end());
    for (size_t index = 0; index < 70; ++index) {
        stable_block_vector.push_back(index);
        size_t count = 0;
        for (auto& element : stable_block_vector) {
            ++count;
            (void)(element);
        }
        EXPECT_EQ(count, index + 1);
        count = 0;
        for (const auto& element : stable_block_vector_const) {
            ++count;
            (void)(element);
        }
        EXPECT_EQ(count, index + 1);
    }
}

namespace {
std::atomic<size_t> open_allocs;
}  // namespace

template<typename T>
class BlockVectorAllocator {
  public:
    using pointer = T*;

    pointer allocate(size_t count)
    {
        ++open_allocs;
        // NOLINTNEXTLINE(hicpp-no-malloc)
        return static_cast<pointer>(std::malloc(count * sizeof(T)));
    }

    void deallocate(pointer ptr, size_t /* count */)
    {
        --open_allocs;
        // NOLINTNEXTLINE(hicpp-no-malloc)
        std::free(ptr);
    }
};

TEST(stableBlockVectorTest, shrink_to_fit)
{
    {
        StableBlockVector<size_t, 3, BlockVectorAllocator<size_t>>
            stable_block_vector(500, 999999U);
        EXPECT_GT(open_allocs.load(), 500 / 8);
        const auto current_allocations = open_allocs.load();
        stable_block_vector.clear();
        EXPECT_EQ(open_allocs.load(), current_allocations);
        stable_block_vector.shrink_to_fit();
        EXPECT_EQ(open_allocs.load(), 1U);
    }
    EXPECT_EQ(open_allocs.load(), 0U);
}

TEST(stableBlockVectorTest, iterator_tests)
{
    StableBlockVector<std::string, 3> stable_block_vector(20);

    for (int index = 0; index < 20; ++index) {
        stable_block_vector[static_cast<size_t>(index)] =
            std::string(30, 'a' + static_cast<char>(index));
    }

    const auto& stable_block_vector_const = stable_block_vector;
    auto iterator = stable_block_vector.begin();
    iterator = iterator + 2;
    EXPECT_EQ((*iterator).front(), 'c');

    auto const_iterator = stable_block_vector_const.begin();
    const_iterator = const_iterator + 2;
    EXPECT_EQ((*const_iterator).front(), 'c');

    iterator += 7;
    const_iterator += 7;
    EXPECT_EQ(iterator->front(), 'j');
    EXPECT_EQ(const_iterator->front(), 'j');

    iterator = iterator - 4;
    const_iterator = const_iterator - 4;
    EXPECT_EQ(iterator->front(), 'f');
    EXPECT_EQ(const_iterator->front(), 'f');
    EXPECT_TRUE(iterator);
    EXPECT_TRUE(const_iterator);
}
