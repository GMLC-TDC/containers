/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once
#include <cstddef>
#include <type_traits>

namespace gmlc
{
namespace containers
{
template <typename X, int BLOCKSIZE, typename OUTER>
/** helper class for iterating through a sequence of blocks*/
class BlockIterator
{
  private:
    OUTER vec;
    X *ptr;
    int offset;

  public:
    using constref = const typename std::remove_const<X>::type;

    BlockIterator(OUTER &it, int startoffset)
        : vec{it}, ptr{&((*it)[startoffset])}, offset{startoffset}
    {
        static_assert(
          std::is_same<std::remove_reference_t<decltype(*(*it))>, X>::value,
          "OUTER *it must be dereferencable to a a type matching X");
    }

    std::enable_if_t<!std::is_const<X>::value, X> &operator*() { return *ptr; }
    constref &operator*() const { return *ptr; }
    std::enable_if_t<!std::is_const<X>::value, X> *operator->() { return ptr; }
    constref *operator->() const { return ptr; }

    operator bool() const { return (ptr != nullptr); }
    bool operator==(const BlockIterator &it) const
    {
        return ((vec == it.vec) && (offset == it.offset));
    }
    bool operator!=(const BlockIterator &it) const
    {
        return ((vec != it.vec) || (offset != it.offset));
    }

    BlockIterator &operator+=(const ptrdiff_t &movement)
    {
        ptr += movement;
        offset += movement;
        check();
        return (*this);
    }
    BlockIterator &operator++()
    {
        ++ptr;
        ++offset;
        check();
        return (*this);
    }
    BlockIterator operator++(int)
    {
        auto temp(*this);
        ++(*this);
        return temp;
    }
    BlockIterator operator+(const ptrdiff_t &movement) const
    {
        auto temp(*this);
        temp += movement;
        return temp;
    }
    BlockIterator &operator-=(const ptrdiff_t &movement)
    {
        ptr -= movement;
        offset -= movement;
        checkNeg();
        return (*this);
    }
    BlockIterator &operator--()
    {
        --ptr;
        --offset;
        checkNeg();
        return (*this);
    }
    BlockIterator operator--(int)
    {
        auto temp(*this);
        --(*this);
        return temp;
    }
    BlockIterator operator-(const ptrdiff_t &movement) const
    {
        auto temp(*this);
        temp -= movement;
        return temp;
    }

  private:
    void check()
    {
        if (offset >= BLOCKSIZE)
        {
            auto diff = offset - BLOCKSIZE;
            vec += 1 + static_cast<size_t>(diff / BLOCKSIZE);
            offset = diff % BLOCKSIZE;
            ptr = &((*vec)[offset]);
        }
    }
    void checkNeg()
    {
        if (offset < 0)
        {
            auto diff = -offset - 1;
            vec -= (1 + static_cast<size_t>(diff / BLOCKSIZE));
            offset = BLOCKSIZE - 1 - (diff % BLOCKSIZE);
            ptr = &((*vec)[offset]);
        }
    }
};

}  // namespace containers
}  // namespace gmlc
