/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance for Sustainable Energy, LLC
All rights reserved. See LICENSE file and DISCLAIMER for more details.
*/
#pragma once

#include "StackBuffer.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace gmlc
{
namespace containers
{
StackBufferRaw::StackBufferRaw (unsigned char *newBlock, int blockSize)
    : origin (newBlock), next (newBlock), dataSize (blockSize)
{
    nextIndex = reinterpret_cast<dataIndex *> (origin + dataSize - sizeof (dataIndex));
}

void StackBufferRaw::swap (StackBufferRaw &other) noexcept
{
    std::swap (origin, other.origin);
    std::swap (next, other.next);
    std::swap (dataSize, other.dataSize);
    std::swap (nextIndex, other.nextIndex);
    std::swap (dataCount, other.dataCount);
}

bool StackBufferRaw::isSpaceAvailable (int sz) const
{
    return (dataSize - (next - origin) - (dataCount + 1) * static_cast<int> (sizeof (dataIndex))) >= sz;
}

bool StackBufferRaw::push (const unsigned char *block, int blockSize)
{
    if (blockSize <= 0)
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

int StackBufferRaw::nextDataSize () const
{
    if (dataCount > 0)
    {
        return nextIndex[1].dataSize;
    }
    return 0;
}

int StackBufferRaw::pop (unsigned char *block, int maxSize)
{
    if (dataCount > 0)
    {
        int blkSize = nextIndex[1].dataSize;
        if (maxSize >= blkSize)
        {
            memcpy (block, origin + nextIndex[1].offset, blkSize);
            if (nextIndex[1].offset + blkSize == static_cast<int> (next - origin))
            {
                next -= blkSize;
            }
            ++nextIndex;
            --dataCount;
            if (dataCount == 0)
            {
                next = origin;
                nextIndex = reinterpret_cast<dataIndex *> (origin + dataSize - sizeof (dataIndex));
            }
            return blkSize;
        }
    }
    return 0;
}

/** reverse the order in which the data will be extracted*/
void StackBufferRaw::reverse ()
{
    if (dataCount <= 1)
    {
        return;
    }
    std::reverse (nextIndex + 1, nextIndex + dataCount + 1);
}

void StackBufferRaw::clear ()
{
    next = origin;
    dataCount = 0;
    nextIndex = reinterpret_cast<dataIndex *> (origin + dataSize - sizeof (dataIndex));
}

StackBuffer::StackBuffer () noexcept : stack (nullptr, 0){};
StackBuffer::StackBuffer (int size)
    : data (reinterpret_cast<unsigned char *> (std::malloc (size))), actualSize{size}, actualCapacity{size},
      stack (data, size)
{
}

StackBuffer::StackBuffer (StackBuffer &&sq) noexcept
    : data (sq.data), actualSize (sq.actualSize), actualCapacity (sq.actualCapacity), stack (std::move (sq.stack))
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

StackBuffer::StackBuffer (const StackBuffer &sq)
    : data{reinterpret_cast<unsigned char *> (std::malloc (sq.actualSize))}, actualSize{sq.actualSize},
      actualCapacity{sq.actualSize}, stack (sq.stack)
{
    memcpy (data, sq.data, actualSize);
    auto offset = stack.next - stack.origin;
    stack.origin = data;
    stack.next = stack.origin + offset;
    stack.nextIndex = reinterpret_cast<dataIndex *> (stack.origin + stack.dataSize - sizeof (dataIndex));
    stack.nextIndex -= stack.dataCount;
}

StackBuffer &StackBuffer::operator= (StackBuffer &&sq) noexcept
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

StackBuffer &StackBuffer::operator= (const StackBuffer &sq)
{
    stack = sq.stack;
    resizeMemory (sq.actualSize);
    std::memcpy (data, sq.data, sq.actualSize);

    auto offset = stack.next - stack.origin;
    stack.origin = data;
    stack.next = stack.origin + offset;
    stack.nextIndex = reinterpret_cast<dataIndex *> (stack.origin + stack.dataSize - sizeof (dataIndex));
    stack.nextIndex -= stack.dataCount;
    return *this;
}

void StackBuffer::resize (int newsize)
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
        int indexOffset = stack.dataSize - sizeof (dataIndex) * stack.dataCount;
        int newOffset = newsize - sizeof (dataIndex) * stack.dataCount;
        memmove (data + newOffset, data + indexOffset, sizeof (dataIndex) * stack.dataCount);
        stack.dataSize = newsize;
        stack.origin = data;
        stack.next = stack.origin + newsize;
        stack.nextIndex = reinterpret_cast<dataIndex *> (stack.origin + stack.dataSize - sizeof (dataIndex));
        stack.nextIndex -= stack.dataCount;
    }
    else  // smaller size
    {
        int indexOffset = stack.dataSize - sizeof (dataIndex) * stack.dataCount;
        int newOffset = newsize - sizeof (dataIndex) * stack.dataCount;
        int dataOffset = static_cast<int> (stack.next - stack.origin);
        if (newsize < dataOffset + sizeof (dataIndex) * stack.dataCount)
        {
            throw (std::runtime_error (
              "unable to resize, current data exceeds new size, please empty stack before resizing"));
        }
        memmove (data + newOffset, data + indexOffset, sizeof (dataIndex) * stack.dataCount);
        stack.dataSize = newsize;
        stack.origin = data;
        stack.next = stack.origin + newsize;
        stack.nextIndex = reinterpret_cast<dataIndex *> (stack.origin + stack.dataSize - sizeof (dataIndex));
        stack.nextIndex -= stack.dataCount;
        actualSize = newsize;
    }
}

void StackBuffer::resizeMemory (int newsize)
{
    if (newsize == actualSize)
    {
        return;
    }
    if (newsize > actualCapacity)
    {
        auto buf = reinterpret_cast<unsigned char *> (std::realloc (data, newsize));
        if (buf == nullptr)
        {
            return;
        }
        data = buf;
        actualCapacity = newsize;
    }
    actualSize = newsize;
}
}  // namespace containers
}  // namespace gmlc
