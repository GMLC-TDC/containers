/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved.

SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace gmlc::containers {
/** A circular buffer for raw data chunks */
class CircularBufferRaw {
  public:
    static constexpr std::ptrdiff_t header_size = sizeof(int);

    CircularBufferRaw(unsigned char* dataBlock, int blockSize) :
        origin(dataBlock), next_write(origin), next_read(origin),
        capacity_(blockSize)
    {
    }

    int capacity() const { return capacity_; }
    bool isSpaceAvailable(int sz) const
    {
        const auto required_space =
            static_cast<std::ptrdiff_t>(sz) + header_size;
        if (next_write >= next_read) {
            const auto remaining_to_end =
                static_cast<std::ptrdiff_t>(capacity_) -
                (next_write - origin);
            if (remaining_to_end >= required_space) {
                return true;
            }
            if ((next_read - origin) >= required_space) {
                return true;
            }
            return false;
        }
        if ((next_read - next_write) >= required_space) {
            return true;
        }
        return false;
    }
    // Return true if push was successful
    bool push(const unsigned char* data, int blockSize)
    {
        if (blockSize <= 0 || data == nullptr) {
            return false;
        }
        const auto required_space =
            static_cast<std::ptrdiff_t>(blockSize) + header_size;
        if (next_write >= next_read) {
            const auto remaining_to_end =
                static_cast<std::ptrdiff_t>(capacity_) -
                (next_write - origin);
            if (remaining_to_end >= required_space) {
                memcpy(next_write, &blockSize, sizeof(int));
                // *(reinterpret_cast<int *>(next_write)) = blockSize;
                memcpy(next_write + 4, data, blockSize);
                next_write += required_space;
                // loop around if there isn't really space for another block
                // of at least 4 bytes and the next_read>origin
                if (((static_cast<std::ptrdiff_t>(capacity_) -
                      (next_write - origin)) <
                     (2 * header_size)) &&
                    (next_read > origin)) {
                    next_write = origin;
                }
                return true;
            } else if ((next_read - origin) >= required_space) {
                int loc = -1;
                memcpy(next_write, &loc, sizeof(int));
                memcpy(origin, &blockSize, sizeof(int));
                memcpy(origin + 4, data, blockSize);
                next_write = origin + blockSize + 4;
                return true;
            }
        } else if ((next_read - next_write) >= required_space) {
            memcpy(next_write, &blockSize, sizeof(int));
            memcpy(next_write + 4, data, blockSize);
            next_write += required_space;
            return true;
        }
        return false;
    }
    int nextDataSize() const
    {
        if (next_write == next_read) {
            return 0;
        }
        int size;
        memcpy(&size, next_read, sizeof(int));
        if (size < 0) {
            memcpy(&size, origin, sizeof(int));
        }
        return size;
    }
    /// Return number of bytes read, 0 if no data available and -nextSize if
    /// insufficient space available
    int pop(unsigned char* data, int maxSize)
    {
        if (next_write == next_read) {
            return 0;
        }
        int size;
        memcpy(&size, next_read, sizeof(int));
        if (size < 0) {
            next_read = origin;
            memcpy(&size, next_read, sizeof(int));
        }
        if (size <= maxSize) {
            memcpy(data, next_read + sizeof(int), size);
            next_read += static_cast<size_t>(size) + sizeof(int);
            if ((static_cast<std::ptrdiff_t>(capacity_) -
                 (next_read - origin)) <
                (2 * header_size)) {
                next_read = origin;
            }
            return size;
        }
        return -size;
    }
    /** check if the block is Empty or not*/
    bool empty() const { return (next_write == next_read); }
    /** clear the buffer*/
    void clear() { next_write = next_read = origin; }

  private:
    unsigned char* origin;
    unsigned char* next_write;
    unsigned char* next_read;
    int capacity_{0};

  private:
    friend class CircularBuffer;
};
/** class implementing a circular buffer with raw memory
 */
class CircularBuffer {
  private:
    static int invalid_size()
    {
        throw(std::invalid_argument("size must be positive"));
    }

  public:
    CircularBuffer() noexcept : buffer(nullptr, 0) {}
    explicit CircularBuffer(int size) :
        data(new unsigned char[size > 0 ? size : invalid_size()]),
        actualSize{size}, actualCapacity{size}, buffer(data, size)
    {
    }
    ~CircularBuffer()
    {
        if (data != nullptr) {
            delete[] data;
        }
    }
    CircularBuffer(CircularBuffer&& cb) noexcept :
        data{cb.data}, actualSize(cb.actualSize),
        actualCapacity(cb.actualCapacity), buffer(std::move(cb.buffer))
    {
        cb.reset();
    }
    CircularBuffer(const CircularBuffer& cb) :
        data{nullptr}, actualSize{cb.actualSize}, actualCapacity{0},
        buffer(nullptr, 0)
    {
        if (cb.data == nullptr || cb.actualSize <= 0) {
            actualSize = 0;
            return;
        }

        data = new unsigned char[cb.actualSize];
        actualCapacity = cb.actualSize;
        memcpy(data, cb.data, actualSize);

        buffer = cb.buffer;
        auto read_offset = buffer.next_read - buffer.origin;
        auto write_offset = buffer.next_write - buffer.origin;
        buffer.origin = data;
        buffer.next_read = buffer.origin + read_offset;
        buffer.next_write = buffer.origin + write_offset;
    }

    CircularBuffer& operator=(CircularBuffer&& cb) noexcept
    {
        if (this != &cb) {
            if (data != nullptr) {
                delete[] data;
            }
            data = cb.data;
            actualSize = cb.actualSize;
            actualCapacity = cb.actualCapacity;
            buffer = std::move(cb.buffer);

            cb.reset();
        }
        return *this;
    }
    CircularBuffer& operator=(const CircularBuffer& cb)
    {
        if (this == &cb) {
            return *this;
        }
        if (cb.data == nullptr || cb.actualSize <= 0) {
            actualSize = 0;
            buffer = CircularBufferRaw(data, 0);
            return *this;
        }

        buffer = cb.buffer;
        resizeMemory(cb.actualSize, false);
        std::memcpy(data, cb.data, cb.actualSize);

        auto read_offset = buffer.next_read - buffer.origin;
        auto write_offset = buffer.next_write - buffer.origin;
        buffer.origin = data;
        buffer.next_read = buffer.origin + read_offset;
        buffer.next_write = buffer.origin + write_offset;
        return *this;
    }

    void resize(int newsize)
    {
        if (newsize <= 0) {
            throw(std::invalid_argument("size must be positive"));
        }
        if (newsize == buffer.capacity_) {
            return;
        }
        if (buffer.empty()) {
            resizeMemory(newsize);
            buffer = CircularBufferRaw(data, newsize);
        } else if (newsize > actualSize) {
            resizeMemory(newsize);
            int read_offset =
                static_cast<int>(buffer.next_read - buffer.origin);
            int write_offset =
                static_cast<int>(buffer.next_write - buffer.origin);
            if (buffer.next_read < buffer.next_write) {
                buffer.capacity_ = newsize;
                buffer.origin = data;
                buffer.next_read = buffer.origin + read_offset;
                buffer.next_write = buffer.origin + write_offset;
            } else {
                int readDiff = buffer.capacity_ - read_offset;
                memmove(
                    data + newsize - readDiff,
                    data + read_offset,
                    static_cast<size_t>(buffer.capacity_) - read_offset);
                buffer.origin = data;
                buffer.next_write = buffer.origin + write_offset;
                buffer.next_read = buffer.origin + newsize - readDiff;
                buffer.capacity_ = newsize;
            }
        } else {  // smaller size
            int read_offset =
                static_cast<int>(buffer.next_read - buffer.origin);
            if (buffer.next_read < buffer.next_write) {
                int write_offset =
                    static_cast<int>(buffer.next_write - buffer.origin);
                if (write_offset <= newsize) {
                    buffer.capacity_ = newsize;
                } else if (write_offset - read_offset < newsize) {
                    memmove(
                        buffer.origin,
                        buffer.next_read,
                        static_cast<std::size_t>(write_offset) - read_offset);
                    buffer.next_read = buffer.origin;
                    buffer.next_write =
                        buffer.origin + write_offset - read_offset;
                    buffer.capacity_ = newsize;
                } else {
                    throw(std::runtime_error(
                        "unable to resize, current data exceeds new size, please "
                        "empty buffer before resizing"));
                }
            } else {
                int write_offset =
                    static_cast<int>(buffer.next_write - buffer.origin);
                int readDiff = buffer.capacity_ - read_offset;
                if (readDiff + write_offset < newsize) {
                    memmove(
                        data + newsize - readDiff,
                        data + read_offset,
                        static_cast<size_t>(buffer.capacity_) - read_offset);
                    buffer.origin = data;
                    buffer.next_write = buffer.origin + write_offset;
                    buffer.next_read = buffer.origin + newsize - readDiff;
                    buffer.capacity_ = newsize;
                } else {
                    throw(std::runtime_error(
                        "unable to resize, current data exceeds new size, please "
                        "empty buffer before resizing"));
                }
            }
            actualSize = newsize;
        }
    }
    int capacity() const { return buffer.capacity(); }
    bool isSpaceAvailable(int sz) const { return buffer.isSpaceAvailable(sz); }
    bool empty() const { return buffer.empty(); }

    bool push(const unsigned char* block, int blockSize)
    {
        return buffer.push(block, blockSize);
    }

    int nextDataSize() const { return buffer.nextDataSize(); }

    int pop(unsigned char* block, int maxSize)
    {
        return buffer.pop(block, maxSize);
    }

    void clear() { buffer.clear(); }

  private:
    void resizeMemory(int newsize, bool copyData = true)
    {
        if (newsize > actualCapacity) {
            auto buf = new unsigned char[newsize];
            if (actualCapacity > 0) {
                if (copyData) {
                    memcpy(buf, data, actualSize);
                }

                delete[] data;
            }
            data = buf;
            actualCapacity = newsize;
        }
        actualSize = newsize;
    }

    void reset() noexcept
    {
        data = nullptr;
        actualSize = 0;
        actualCapacity = 0;
        buffer.capacity_ = 0;
        buffer.origin = nullptr;
        buffer.next_read = nullptr;
        buffer.next_write = nullptr;
    }

  private:
    unsigned char* data{nullptr};
    int actualSize{0};
    int actualCapacity{0};
    CircularBufferRaw buffer;
};

}  // namespace gmlc::containers
