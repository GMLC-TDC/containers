/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <stdexcept>

namespace gmlc
{
namespace containers
{
class CircularBufferRaw
{
  public:
    CircularBufferRaw (unsigned char *dataBlock, int capacity);

    int capacity () const { return capacity_; }
    bool isSpaceAvailable (int sz) const;
    // Return true if push was successful
    bool push (const unsigned char *data, int blockSize);
    int nextDataSize () const;
    // Return number of bytes read.
    int pop (unsigned char *data, int maxSize);
    /** check if the block is Empty or not*/
    bool empty () const;
    /** clear the buffer*/
    void clear ();

  private:
    unsigned char *origin;
    unsigned char *next_write;
    unsigned char *next_read;
    int capacity_ = 0;

  private:
    friend class CircularBuffer;
};
/** class implementing a circular buffer with raw memory
 */
class CircularBuffer
{
  public:
    CircularBuffer () noexcept;
    explicit CircularBuffer (int size);
    ~CircularBuffer () = default;
    CircularBuffer (CircularBuffer &&cb) noexcept;
    CircularBuffer (const CircularBuffer &cb);

    CircularBuffer &operator= (CircularBuffer &&cb) noexcept;
    CircularBuffer &operator= (const CircularBuffer &cb);

    void resize (int newsize);
    int capacity () const { return buffer.capacity (); }
    bool isSpaceAvailable (int sz) const
    {
        return buffer.isSpaceAvailable (sz);
    }
    bool empty () const { return buffer.empty (); }

    bool push (const unsigned char *block, int blockSize)
    {
        return buffer.push (block, blockSize);
    }

    int next_data_size () const { return buffer.nextDataSize (); }

    int pop (unsigned char *block, int maxSize)
    {
        return buffer.pop (block, maxSize);
    }

    void clear () { buffer.clear (); }

  private:
    void resizeMemory (int newsize);

  private:
    unsigned char *data = nullptr;
    int actualSize = 0;
    int actualCapacity = 0;
    CircularBufferRaw buffer;
};

}  // namespace containers
}  // namespace gmlc
