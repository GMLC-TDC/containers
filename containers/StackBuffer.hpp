/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace gmlc
{
namespace containers
{
struct dataIndex
{
    int32_t offset;
    int32_t dataSize;
};

/// Helper for size information on the data index
constexpr int diSize = static_cast<int> (sizeof (dataIndex));

/** class containing the raw StackBuffer implementation
@details the StackBufferRaw class operates on raw memory
it is given a memory location and uses that for the life of the queue, it does
not own the memory so care must be taken for memory management  It operates on
blocks of raw data
*/
class StackBufferRaw
{
  private:
    unsigned char *origin = nullptr;
    unsigned char *next = nullptr;
    dataIndex *nextIndex = nullptr;
    int dataSize = 0;
    int dataCount = 0;

  public:
    StackBufferRaw (unsigned char *newBlock, int blockSize)
        : origin (newBlock), next (newBlock), dataSize (blockSize)
    {
        nextIndex = reinterpret_cast<dataIndex *> (origin + dataSize - diSize);
    }

    void swap (StackBufferRaw &other) noexcept
    {
        std::swap (origin, other.origin);
        std::swap (next, other.next);
        std::swap (dataSize, other.dataSize);
        std::swap (nextIndex, other.nextIndex);
        std::swap (dataCount, other.dataCount);
    }

    int capacity () const { return dataSize; };
    int getCurrentCount () const { return dataCount; }
    bool isSpaceAvailable (int sz) const
    {
        return (dataSize - (next - origin) - (dataCount + 1) * diSize) >=
               static_cast<ptrdiff_t> (sz);
    }
    bool empty () const { return (dataCount == 0); }

    bool push (const unsigned char *block, int blockSize)
    {
        if (blockSize <= 0 || block == nullptr)
        {
            return false;
        }
        if (!isSpaceAvailable (blockSize))
        {
            return false;
        }
        memcpy (next, block, blockSize);
        nextIndex->offset = static_cast<int> (next - origin);
        nextIndex->dataSize = blockSize;
        next += blockSize;
        --nextIndex;
        ++dataCount;
        return true;
    }

    int nextDataSize () const
    {
        return (dataCount > 0) ? nextIndex[1].dataSize : 0;
    }

    int pop (unsigned char *block, int maxSize)
    {
        if (dataCount > 0)
        {
            int blkSize = nextIndex[1].dataSize;
            if (maxSize >= blkSize)
            {
                memcpy (block, origin + nextIndex[1].offset, blkSize);
                if (nextIndex[1].offset + blkSize ==
                    static_cast<int> (next - origin))
                {
                    next -= blkSize;
                }
                ++nextIndex;
                --dataCount;
                if (dataCount == 0)
                {
                    next = origin;
                    nextIndex = reinterpret_cast<dataIndex *> (
                      origin + dataSize - diSize);
                }
                return blkSize;
            }
        }
        return 0;
    }

    /** reverse the order in which the data will be extracted*/
    void reverse ()
    {
        if (dataCount > 1)
        {
            std::reverse (nextIndex + 1, nextIndex + dataCount + 1);
        }
    }
    /** clear all data from the StackBufferRaw*/
    void clear ()
    {
        next = origin;
        dataCount = 0;
        nextIndex = reinterpret_cast<dataIndex *> (origin + dataSize - diSize);
    }

  private:
    friend class StackBuffer;
};

/** StackBuffer manages memory for a StackBufferRaw and adds some convenience
 * functions */
class StackBuffer
{
  public:
    StackBuffer () noexcept : stack (nullptr, 0) {}
    explicit StackBuffer (int size)
        : data (reinterpret_cast<unsigned char *> (std::malloc (size))),
          actualSize{size}, actualCapacity{size}, stack (data, size)
    {
    }
    ~StackBuffer () = default;
    StackBuffer (StackBuffer &&sq) noexcept
        : data (sq.data), actualSize (sq.actualSize),
          actualCapacity (sq.actualCapacity), stack (std::move (sq.stack))
    {
        sq.data = nullptr;
        sq.actualSize = 0;
        sq.actualCapacity = 0;
        sq.stack.dataSize = 0;
        sq.stack.origin = nullptr;
        sq.stack.next = nullptr;
        sq.stack.dataCount = 0;
        sq.stack.nextIndex = nullptr;
    }
    StackBuffer (const StackBuffer &sq)
        : data{reinterpret_cast<unsigned char *> (std::malloc (sq.actualSize))},
          actualSize{sq.actualSize}, actualCapacity{sq.actualSize},
          stack (sq.stack)
    {
        memcpy (data, sq.data, static_cast<size_t> (actualSize));
        auto offset = stack.next - stack.origin;
        stack.origin = data;
        stack.next = stack.origin + offset;
        stack.nextIndex = reinterpret_cast<dataIndex *> (
          stack.origin + stack.dataSize - diSize);
        stack.nextIndex -= stack.dataCount;
    }

    StackBuffer &operator= (StackBuffer &&sq) noexcept
    {
        if (data != nullptr)
        {
            std::free (data);
        }
        data = sq.data;
        actualSize = sq.actualSize;
        actualCapacity = sq.actualCapacity;
        stack = std::move (sq.stack);
        sq.data = nullptr;
        sq.actualSize = 0;
        sq.stack.dataSize = 0;
        sq.stack.origin = nullptr;
        sq.stack.next = nullptr;
        sq.stack.dataCount = 0;
        sq.stack.nextIndex = nullptr;
        return *this;
    }
    StackBuffer &operator= (const StackBuffer &sq)
    {
        stack = sq.stack;
        resizeMemory (sq.actualSize);
        std::memcpy (data, sq.data, sq.actualSize);

        auto offset = stack.next - stack.origin;
        stack.origin = data;
        stack.next = stack.origin + offset;
        stack.nextIndex = reinterpret_cast<dataIndex *> (
          stack.origin + stack.dataSize - diSize);
        stack.nextIndex -= stack.dataCount;
        return *this;
    }

    void resize (int newsize)
    {
        if (newsize == stack.dataSize)
        {
            return;
        }
        if (stack.dataCount == 0)
        {
            resizeMemory (newsize);
            stack = StackBufferRaw (data, newsize);
        }
        else if (newsize > actualSize)
        {
            resizeMemory (newsize);
            int indexOffset = stack.dataSize - diSize * stack.dataCount;
            int newOffset = newsize - diSize * stack.dataCount;
            memmove (data + newOffset, data + indexOffset,
                     static_cast<size_t> (diSize) *
                       static_cast<size_t> (stack.dataCount));
            stack.dataSize = newsize;
            stack.origin = data;
            stack.next = stack.origin + newsize;
            stack.nextIndex = reinterpret_cast<dataIndex *> (
              stack.origin + stack.dataSize - diSize);
            stack.nextIndex -= stack.dataCount;
        }
        else  // smaller size
        {
            int indexOffset = stack.dataSize - diSize * stack.dataCount;
            int newOffset = newsize - diSize * stack.dataCount;
            int dataOffset = static_cast<int> (stack.next - stack.origin);
            if (newsize < dataOffset + diSize * stack.dataCount)
            {
                throw (std::runtime_error (
                  "unable to resize, current data exceeds new "
                  "size, please empty stack before resizing"));
            }
            memmove (data + newOffset, data + indexOffset,
                     static_cast<size_t> (diSize) *
                       static_cast<size_t> (stack.dataCount));
            stack.dataSize = newsize;
            stack.origin = data;
            stack.next = stack.origin + newsize;
            stack.nextIndex = reinterpret_cast<dataIndex *> (
              stack.origin + stack.dataSize - diSize);
            stack.nextIndex -= stack.dataCount;
            actualSize = newsize;
        }
    }
    int getCurrentCount () const { return stack.getCurrentCount (); }
    int capacity () const { return stack.capacity (); }
    bool isSpaceAvailable (int sz) const { return stack.isSpaceAvailable (sz); }
    bool empty () const { return stack.empty (); }

    bool push (const unsigned char *block, int blockSize)
    {
        return stack.push (block, blockSize);
    }

    int nextDataSize () const { return stack.nextDataSize (); }

    int pop (unsigned char *block, int maxSize)
    {
        return stack.pop (block, maxSize);
    }

    void reverse () { stack.reverse (); }
    void clear () { stack.clear (); }

    void swap (StackBuffer &other) noexcept
    {
        stack.swap (other.stack);
        std::swap (data, other.data);
        std::swap (actualCapacity, other.actualCapacity);
        std::swap (actualSize, other.actualSize);
    }

  private:
    void resizeMemory (int newsize)
    {
        if (newsize == actualSize)
        {
            return;
        }
        if (newsize > actualCapacity)
        {
            auto buf =
              reinterpret_cast<unsigned char *> (std::realloc (data, newsize));
            if (buf == nullptr)
            {
                return;
            }
            data = buf;
            actualCapacity = newsize;
        }
        actualSize = newsize;
    }

  private:
    unsigned char *data = nullptr;  //!< pointer to the memory data block
    int actualSize = 0;  //!< the stated size of the memory block
    int actualCapacity = 0;  //!< the actual size of the memory block
    StackBufferRaw stack;  //!< The actual stack controller
};

}  // namespace containers
}  // namespace gmlc
