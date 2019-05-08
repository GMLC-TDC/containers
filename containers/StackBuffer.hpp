/*
Copyright © 2017-2018,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC All rights reserved. See LICENSE file and DISCLAIMER
for more details.
*/
#pragma once

#include <cstdint>

namespace gmlc
{
namespace containers
{
struct dataIndex
{
    int32_t offset;
    int32_t dataSize;
};
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
    StackBufferRaw (unsigned char *newBlock, int blockSize);

    void swap (StackBufferRaw &other) noexcept;

    int capacity () const { return dataSize; };
    int getCurrentCount () const { return dataCount; }
    bool isSpaceAvailable (int sz) const;
    bool empty () const { return (dataCount == 0); }

    bool push (const unsigned char *block, int blockSize);

    int nextDataSize () const;

    int pop (unsigned char *block, int maxSize);

    /** reverse the order in which the data will be extracted*/
    void reverse ();
    /** clear all data from the StackBufferRaw*/
    void clear ();

  private:
    friend class StackBuffer;
};

/** StackBuffer manages memory for a StackBufferRaw and adds some convenience
 * functions */
class StackBuffer
{
  public:
    StackBuffer () noexcept;
    explicit StackBuffer (int size);
    ~StackBuffer () = default;
    StackBuffer (StackBuffer &&sq) noexcept;
    StackBuffer (const StackBuffer &sq);

    StackBuffer &operator= (StackBuffer &&sq) noexcept;
    StackBuffer &operator= (const StackBuffer &sq);

    void resize (int newsize);
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

  private:
    void resizeMemory (int newsize);

  private:
    unsigned char *data = nullptr;  //!< pointer to the memory data block
    int actualSize = 0;  //!< the stated size of the memory block
    int actualCapacity = 0;  //!< the actual size of the memory block
    StackBufferRaw stack;  //!< The actual stack controller
};

}  // namespace containers
}  // namespace gmlc
