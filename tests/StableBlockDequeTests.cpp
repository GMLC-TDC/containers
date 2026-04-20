/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "StableBlockDeque.hpp"

#include "gtest/gtest.h"
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

using gmlc::containers::StableBlockDeque;

TEST(stableBlockDequeTest, test_lookup)
{
    StableBlockDeque<std::string, 2> stable_block_deque;
    stable_block_deque.push_back("bob");
    stable_block_deque.emplace_back("bob2");
    stable_block_deque.emplace_back("bob3");
    stable_block_deque.emplace_back("bob4");
    stable_block_deque.emplace_back("bob5");
    EXPECT_EQ(stable_block_deque.size(), 5);
    EXPECT_EQ(stable_block_deque[0], "bob");
    EXPECT_EQ(stable_block_deque[1], "bob2");
    EXPECT_EQ(stable_block_deque[2], "bob3");
    EXPECT_EQ(stable_block_deque[3], "bob4");
    EXPECT_EQ(stable_block_deque[4], "bob5");
}

TEST(stableBlockDequeTest, test_lookup_front)
{
    StableBlockDeque<std::string, 2> stable_block_deque;
    stable_block_deque.push_front("bob5");
    stable_block_deque.emplace_front("bob4");
    stable_block_deque.emplace_front("bob3");
    stable_block_deque.emplace_front("bob2");
    stable_block_deque.emplace_front("bob");
    EXPECT_EQ(stable_block_deque.size(), 5);
    EXPECT_EQ(stable_block_deque[0], "bob");
    EXPECT_EQ(stable_block_deque[1], "bob2");
    EXPECT_EQ(stable_block_deque[2], "bob3");
    EXPECT_EQ(stable_block_deque[3], "bob4");
    EXPECT_EQ(stable_block_deque[4], "bob5");
}

TEST(stableBlockDequeTest, test_const_lookup)
{
    StableBlockDeque<std::string, 2> stable_block_vector;
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
}

TEST(stableBlockDequeTest, test_iterator)
{
    StableBlockDeque<std::string, 2> stable_block_deque;
    stable_block_deque.push_back("bob");
    stable_block_deque.emplace_back("bob2");
    stable_block_deque.emplace_back("bob3");
    stable_block_deque.emplace_back("bob4");
    stable_block_deque.emplace_back("bob5");
    EXPECT_EQ(stable_block_deque.size(), 5);
    auto iterator = stable_block_deque.begin();
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
    while (iterator != stable_block_deque.end()) {
        ++index;
        ++iterator;
    }
    EXPECT_EQ(index, 5);
}

TEST(stableBlockDequeTest, test_iterator2)
{
    StableBlockDeque<std::string, 2> stable_block_vector;
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

TEST(stableBlockDequeTest, test_const_iterator)
{
    StableBlockDeque<std::string, 2> stable_block_vector;
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

TEST(stableBlockDequeTest, test_const_iterator2)
{
    StableBlockDeque<std::string, 2> stable_block_vector;
    stable_block_vector.push_back("bob");
    stable_block_vector.emplace_back("bob2");
    stable_block_vector.emplace_back("bob3");
    stable_block_vector.emplace_back("bob4");
    const auto& stable_block_vector_const = stable_block_vector;
    EXPECT_EQ(stable_block_vector_const.size(), 4);
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
}

TEST(stableBlockDequeTest, test_simple)
{
    StableBlockDeque<std::string, 2> stable_block_deque;

    stable_block_deque.push_back("bob");
    stable_block_deque.emplace_back("bob2");
    stable_block_deque.emplace_back("bob3");
    stable_block_deque.emplace_back("bob4");
    stable_block_deque.emplace_back("bob5");
    stable_block_deque.emplace_back("bob6");
    stable_block_deque.push_back("bob7");
    stable_block_deque.emplace_back("bob8");
    stable_block_deque.emplace_back("bob9");
    stable_block_deque.emplace_back("bob10");
    stable_block_deque.push_back("bob11");
    stable_block_deque.emplace_back("bob12");
    stable_block_deque.emplace_back("bob13");
    stable_block_deque.emplace_back("bob14");
    int index = 0;
    for (const auto& element : stable_block_deque) {
        (void)(element);
        ++index;
    }
    EXPECT_EQ(stable_block_deque.size(), 14);
    EXPECT_EQ(index, 14);

    stable_block_deque.pop_back();
    stable_block_deque.pop_back();
    stable_block_deque.pop_back();
    stable_block_deque.pop_back();
    auto& stable_block_deque_alias = stable_block_deque;
    index = 0;
    for (const auto& element : stable_block_deque_alias) {
        (void)(element);
        ++index;
    }
    EXPECT_EQ(stable_block_deque.size(), 10);
    EXPECT_EQ(stable_block_deque_alias.size(), 10);
    EXPECT_EQ(index, 10);
}

TEST(stableBlockDequeTest, iterator_check)
{
    for (size_t size = 1; size < 120; ++size) {
        StableBlockDeque<std::string, 4> stable_block_deque(size);
        const auto& stable_block_deque_copy = stable_block_deque;
        auto iterator = stable_block_deque.begin();
        size_t index = 0;
        while (iterator != stable_block_deque.end()) {
            *iterator = std::to_string(index);
            ++iterator;
            ++index;
        }
        EXPECT_EQ(index, size);
        for (index = 0; index < size; ++index) {
            const auto text = std::to_string(index);
            const auto& result = stable_block_deque[index];
            EXPECT_EQ(text, result);
            EXPECT_EQ(text, stable_block_deque_copy[index]);
        }
    }
}

TEST(stableBlockDequeTest, test_start)
{
    StableBlockDeque<std::string, 4> stable_block_deque(367, "frog");
    EXPECT_EQ(stable_block_deque.size(), 367U);

    EXPECT_EQ(stable_block_deque[231], "frog");
    EXPECT_EQ(stable_block_deque[0], "frog");
    EXPECT_EQ(stable_block_deque[366], "frog");

    const StableBlockDeque<std::string, 2> stable_block_deque2(0, "frog");
    EXPECT_EQ(stable_block_deque2.size(), 0U);
    StableBlockDeque<std::string, 2> stable_block_deque3(20);
    EXPECT_EQ(stable_block_deque3.size(), 20U);
    EXPECT_TRUE(stable_block_deque3[19].empty());
    EXPECT_TRUE(stable_block_deque3[0].empty());
}

TEST(stableBlockDequeTest, test_front_back)
{
    StableBlockDeque<size_t, 4> stable_block_deque(200);
    const auto& stable_block_deque_copy = stable_block_deque;
    size_t index = 0;
    for (auto& value : stable_block_deque) {
        value = index++;
    }
    EXPECT_EQ(stable_block_deque_copy.front(), 0U);
    EXPECT_EQ(stable_block_deque_copy.back(), 199U);
    for (index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque.front(), index);
        EXPECT_EQ(stable_block_deque.back(), 199 - index);
        EXPECT_EQ(stable_block_deque_copy[0], index);
        EXPECT_EQ(*stable_block_deque_copy.begin(), index);

        stable_block_deque.pop_front();
        stable_block_deque.pop_back();
    }

    EXPECT_NO_THROW(stable_block_deque.pop_front());
    EXPECT_NO_THROW(stable_block_deque.pop_back());
}

TEST(stableBlockDequeTest, move_construct)
{
    auto* stable_block_deque = new StableBlockDeque<size_t, 4>(200);
    size_t index = 0;
    for (auto& value : *stable_block_deque) {
        value = index++;
    }
    StableBlockDeque<size_t, 4> stable_block_deque2(std::move(*stable_block_deque));
    delete stable_block_deque;
    EXPECT_EQ(stable_block_deque2.size(), 200);
    for (index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque2.front(), index);
        EXPECT_EQ(stable_block_deque2.back(), 199 - index);

        stable_block_deque2.pop_front();
        stable_block_deque2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_construct)
{
    StableBlockDeque<size_t, 4> stable_block_deque(200);
    size_t index = 0;
    for (auto& value : stable_block_deque) {
        value = index++;
    }
    StableBlockDeque<size_t, 4> stable_block_deque2(stable_block_deque);

    EXPECT_EQ(stable_block_deque2.size(), stable_block_deque.size());
    for (index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque2.front(), stable_block_deque[index]);
        EXPECT_EQ(stable_block_deque2.back(), stable_block_deque[199 - index]);

        stable_block_deque2.pop_front();
        stable_block_deque2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_construct_empty)
{
    StableBlockDeque<size_t, 4> stable_block_deque;
    EXPECT_TRUE(stable_block_deque.empty());
    stable_block_deque.clear();
    EXPECT_TRUE(stable_block_deque.empty());
    StableBlockDeque<size_t, 4> stable_block_deque2(stable_block_deque);

    EXPECT_EQ(stable_block_deque2.size(), stable_block_deque.size());
    EXPECT_EQ(stable_block_deque2.size(), 0);
    stable_block_deque2.clear();
    EXPECT_EQ(stable_block_deque2.size(), stable_block_deque.size());
    EXPECT_EQ(stable_block_deque2.size(), 0);
}

TEST(stableBlockDequeTest, move_assign)
{
    StableBlockDeque<size_t, 4> stable_block_deque2(100);
    {
        StableBlockDeque<size_t, 4> stable_block_deque(200);
        size_t index = 0;
        for (auto& value : stable_block_deque) {
            value = index++;
        }
        stable_block_deque2 = std::move(stable_block_deque);
    }
    EXPECT_EQ(stable_block_deque2.size(), 200);
    for (size_t index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque2.front(), index);
        EXPECT_EQ(stable_block_deque2.back(), 199 - index);

        stable_block_deque2.pop_front();
        stable_block_deque2.pop_back();
    }
}

TEST(stableBlockDequeTest, assign_move)
{
    std::vector<std::string> vector1(200, std::string(200, 'a'));
    StableBlockDeque<std::string, 5> stable_block_deque2(100, std::string(100, 'b'));

    stable_block_deque2.move_assign(vector1.begin(), vector1.end());
    EXPECT_EQ(stable_block_deque2.size(), 200);
    for (size_t index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque2.front(), std::string(200, 'a'));
        EXPECT_EQ(stable_block_deque2.back(), std::string(200, 'a'));

        stable_block_deque2.pop_front();
        stable_block_deque2.pop_back();
    }
}

TEST(stableBlockDequeTest, assign_move_to_empty)
{
    std::vector<std::string> vector1(200, std::string(200, 'a'));
    StableBlockDeque<std::string, 5> stable_block_deque2;

    stable_block_deque2.move_assign(vector1.begin(), vector1.end());
    EXPECT_EQ(stable_block_deque2.size(), 200);
    for (size_t index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque2.front(), std::string(200, 'a'));
        EXPECT_EQ(stable_block_deque2.back(), std::string(200, 'a'));

        stable_block_deque2.pop_front();
        stable_block_deque2.pop_back();
    }
}

TEST(stableBlockDequeTest, assign_move_to_bigger)
{
    std::vector<std::string> vector1(200, std::string(200, 'a'));
    StableBlockDeque<std::string, 5> stable_block_deque2(500, std::string(100, 'b'));

    stable_block_deque2.move_assign(vector1.begin(), vector1.end());
    EXPECT_EQ(stable_block_deque2.size(), 200);
    for (size_t index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque2.front(), std::string(200, 'a'));
        EXPECT_EQ(stable_block_deque2.back(), std::string(200, 'a'));

        stable_block_deque2.pop_front();
        stable_block_deque2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_assign)
{
    StableBlockDeque<size_t, 4> stable_block_deque2(100);

    StableBlockDeque<size_t, 4> stable_block_deque(200);
    size_t index = 0;
    for (auto& value : stable_block_deque) {
        value = index++;
    }
    stable_block_deque2 = stable_block_deque;

    EXPECT_EQ(stable_block_deque2.size(), 200);
    for (index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque2.front(), stable_block_deque[index]);
        EXPECT_EQ(stable_block_deque2.back(), stable_block_deque[199 - index]);

        stable_block_deque2.pop_front();
        stable_block_deque2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_assign_from_empty)
{
    StableBlockDeque<size_t, 4> stable_block_deque2;

    StableBlockDeque<size_t, 4> stable_block_deque(200);
    size_t index = 0;
    for (auto& value : stable_block_deque) {
        value = index++;
    }
    stable_block_deque2 = stable_block_deque;

    EXPECT_EQ(stable_block_deque2.size(), 200);
    for (index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque2.front(), stable_block_deque[index]);
        EXPECT_EQ(stable_block_deque2.back(), stable_block_deque[199 - index]);

        stable_block_deque2.pop_front();
        stable_block_deque2.pop_back();
    }
}

TEST(stableBlockDequeTest, copy_assign_to_bigger)
{
    StableBlockDeque<size_t, 4> stable_block_deque2(400, 5);

    StableBlockDeque<size_t, 4> stable_block_deque(200);
    size_t index = 0;
    for (auto& value : stable_block_deque) {
        value = index++;
    }
    stable_block_deque2 = stable_block_deque;

    EXPECT_EQ(stable_block_deque2.size(), 200);
    for (index = 0; index < 100; ++index) {
        EXPECT_EQ(stable_block_deque2.front(), stable_block_deque[index]);
        EXPECT_EQ(stable_block_deque2.back(), stable_block_deque[199 - index]);

        stable_block_deque2.pop_front();
        stable_block_deque2.pop_back();
    }
}

TEST(stableBlockDequeTest, clear_and_fill)
{
    StableBlockDeque<size_t, 4> stable_block_deque(200);
    EXPECT_EQ(stable_block_deque.size(), 200U);
    stable_block_deque.clear();
    EXPECT_EQ(stable_block_deque.size(), 0U);
    for (size_t index = 0; index < 200; ++index) {
        stable_block_deque.push_back(index);
        stable_block_deque.push_front(index);
    }
    EXPECT_EQ(stable_block_deque.size(), 400U);
}

TEST(stableBlockDequeTest, fill_large_back)
{
    StableBlockDeque<size_t, 4> stable_block_deque(200, 999999999UL);
    for (size_t index = 0; index < 10000; ++index) {
        stable_block_deque.push_back(index);
    }

    EXPECT_EQ(stable_block_deque.size(), 10200);
    for (size_t index = 0; index < 10000; ++index) {
        stable_block_deque.pop_back();
    }
    EXPECT_EQ(stable_block_deque.size(), 200);
}

TEST(stableBlockDequeTest, fill_large_front)
{
    StableBlockDeque<size_t, 4> stable_block_deque(200, 999999999UL);
    for (size_t index = 0; index < 10000; ++index) {
        stable_block_deque.push_front(index);
    }

    EXPECT_EQ(stable_block_deque.size(), 10200);
    for (size_t index = 0; index < 10000; ++index) {
        stable_block_deque.pop_front();
    }
    EXPECT_EQ(stable_block_deque.size(), 200);
}

TEST(stableBlockDequeTest, iterators)
{
    StableBlockDeque<size_t, 3> stable_block_deque;
    EXPECT_TRUE(stable_block_deque.begin() == stable_block_deque.end());
    const auto& stable_block_deque_const = stable_block_deque;
    EXPECT_TRUE(stable_block_deque_const.begin() == stable_block_deque_const.end());
    for (size_t index = 0; index < 70; ++index) {
        stable_block_deque.push_back(index);
        size_t count = 0;
        for (auto& element : stable_block_deque) {
            ++count;
            (void)(element);
        }
        EXPECT_EQ(count, index + 1);
        count = 0;
        for (const auto& element : stable_block_deque_const) {
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
class BlockDequeAllocator {
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

TEST(stableBlockDequeTest, shrink_to_fit)
{
    {
        StableBlockDeque<size_t, 3, BlockDequeAllocator<size_t>> stable_block_deque(
            500,
            999999U);
        EXPECT_GT(open_allocs.load(), 500 / 8);
        const auto current_allocations = open_allocs.load();
        stable_block_deque.clear();
        EXPECT_EQ(open_allocs.load(), current_allocations);
        stable_block_deque.shrink_to_fit();
        EXPECT_EQ(open_allocs.load(), 1U);
    }
    EXPECT_EQ(open_allocs.load(), 0U);
}
