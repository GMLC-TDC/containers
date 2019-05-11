/*
Copyright © 2017-2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include "BlockIterator.hpp"
#include <memory>
#include <type_traits>

namespace gmlc
{
namespace containers
{
/** class to function similarly to a deque  with push/ppp back and front
But the memory is not contiguous and the elements are stable
no erase or insert
@tparam X the type to store in the vector
@tparam N the blocksize exponent store 2^N objects in an allocation block
@tparam Allocator and allocator object to get the blocks*/
template <typename X, unsigned int N, class Allocator = std::allocator<X>>
class StableBlockDeque
{
    static_assert (
      N < 32,
      "N should be between 0 and 31 data will allocated in block 2^N");
    static_assert (std::is_default_constructible<X>::value,
                   " used type must be default constructible");

  public:
    static constexpr unsigned int blockSize{1u << N};
    using iterator = BlockIterator<X, (1u << N), X **>;
    using const_iterator = BlockIterator<const X, (1u << N), X *const *>;

  private:
    static constexpr unsigned int cntmask{blockSize - 1};

  public:
    /** constructor*/
    StableBlockDeque () noexcept {};

    /** construct with a specified size*/
    StableBlockDeque (size_t startSize, const X &init = X{})
        : csize (startSize),
          dataptr (new X *[std::max ((startSize >> N) + 2, 64)]),
          dataSlotsAvailable (std::max ((startSize >> N) + 2, 64)),
    {
        if (startSize == 0)
        {
            dataSlotFront = dataSlotBack = 30;
            dataptr[30] = a.allocate (blockSize);
            bsize = fsize = blockSize / 2;
        }
        else
        {
            dataSlotFront = (dataSlotsAvailable - (startSize >> N)) / 2;
            dataSlotBack = dataSlotFront + (startSize >> N);
            auto med = startSize - (startSize >> N) * blockSize;
            fsize = blockSize - (med / 2);
            bsize = med - (med / 2);
            dataptr[dataSlotFront] = a.allocate (blockSize);
            for (int jj = fsize + 1; jj < blockSize; ++jj)
            {
                new (&dataptr[dataSlotFront][jj]) X{init};
            }
            for (int ii = dataSlotFront + 1; ii <= dataSlotBack - 1; ++ii)
            {
                dataptr[ii] = a.allocate (blockSize);
                for (int jj = 0; jj < blockSize; ++jj)
                {
                    new (&dataptr[ii][jj]) X{init};
                }
            }
            if (dataSlotBack > dataSlotFront)
            {
                for (int jj = 0; jj < bsize; ++jj)
                {
                    new (&dataptr[dataSlotBack][jj]) X{init};
                }
            }
        }
    }

    StableBlockDeque (const StableBlockDeque &sbd) : StableBlockDeque ()
    {
        if (sbd.dataptr != nullptr)
        {
            dataptr = new X *[sbd.dataSlotsAvailable];
            dataSlotsAvailable = sbd.dataSlotsAvailable;
            dataSlotFront = sbd.dataSlotFront;
            fsize = sdb.fsize;
            bsize = sbd.fsize + 1;
            dataptr[dataSlotFront] = getNewBlock ();
            assign (sbd.begin (), sbd.end ());
        }
    }
    StableBlockDeque (StableBlockDeque &&sbd) noexcept
        : csize (sbd.csize), dataptr (sbd.dataptr),
          dataSlotsAvailable (sbd.dataSlotsAvailable),
          dataSlotIndex (sbd.dataSlotIndex), bsize (sbd.bsize),
          freeSlotsAvailable (sbd.freeSlotsAvailable),
          freeIndex (sbd.freeIndex), freeblocks (sbd.freeblocks)
    {
        sbd.freeblocks = nullptr;
        sbd.freeSlotsAvailable = 0;
        sbd.dataSlotsAvailable = 0;
        sbd.dataptr = nullptr;
    }

    StableBlockDeque &operator= (const StableBlockDeque &sbd)
    {
        clear ();
        assign (sbd.begin (), sbd.end ());
        return *this;
    }
    StableBlockDeque &operator= (StableBlockDeque &&sbd) noexcept
    {
        csize = sbd.csize;
        dataptr = sbd.dataptr;
        dataSlotsAvailable = sbd.dataSlotsAvailable;
        dataSlotFront = sbd.dataSlotFront;
        dataSlotBack = sbd.dataSlotBack;
        bsize = sbd.bsize;
        fsize = sbd.fsize;
        freeSlotsAvailable = sbd.freeSlotsAvailable;
        freeIndex = sbd.freeIndex;
        freeblocks = sbd.freeblocks;

        sbd.freeblocks = nullptr;
        sbd.freeSlotsAvailable = 0;
        sbd.dataSlotsAvailable = 0;
        sbd.dataptr = nullptr;

        return *this;
    }
    /** destructor*/
    ~StableBlockDeque ()
    {
        clear ();
        Allocator a;
        a.deallocate (dataptr[dataSlotFront], blockSize);
        for (int ii = 0; ii < freeIndex; ++ii)
        {
            a.deallocate (freeblocks[ii], blockSize);
        }
        delete[] freeblocks;
        delete[] dataptr;
    }

    template <class... Args>
    void emplace_back (Args &&... args)
    {
        blockCheck ();
        new (&(dataptr[dataSlotBack][bsize++]))
          X (std::forward<Args> (args)...);
        ++csize;
    }

    void push_back (const X &val)
    {
        blockCheck ();
        new (&(dataptr[dataSlotBack][bsize++])) X{val};
        ++csize;
    }
    void push_back (X &&val)
    {
        blockCheck ();
        new (&(dataptr[dataSlotBack][bsize++])) X{std::move (val)};
        ++csize;
    }

    void pop_back ()
    {
        if (csize == 0)
        {
            return;
        }
        --csize;
        if (bsize > 0)
        {
            --bsize;
        }
        else
        {
            moveBlocktoAvailable (dataptr[dataSlotBack--]);
            bsize = blockSize - 1;
        }
        dataptr[dataSlotBack][bsize].~X ();
    }

    template <class... Args>
    void emplace_front (Args &&... args)
    {
        blockCheckFront ();
        new (&(dataptr[dataSlotFront][fsize--]))
          X (std::forward<Args> (args)...);
        ++csize;
    }

    void push_front (const X &val)
    {
        blockCheckFront ();
        new (&(dataptr[dataSlotFront][fsize--])) X{val};
        ++csize;
    }
    void push_front (X &&val)
    {
        blockCheckFront ();
        new (&(dataptr[dataSlotFront][fsize--])) X{std::move (val)};
        ++csize;
    }

    void pop_front ()
    {
        if (csize == 0)
        {
            return;
        }
        --csize;
        if (fsize != blockSize - 1)
        {
            ++fsize;
        }
        else
        {
            moveBlocktoAvailable (dataptr[dataSlotFront++]);
            fsize = 0;
        }
        dataptr[dataSlotFront][bsize].~X ();
    }

    template <class InputIt>
    void assign (InputIt first, InputIt last)
    {
    }

    template <class InputIt>
    void move_assign (InputIt first, InputIt last)
    {
    }

    void clear () noexcept (std::is_nothrow_destructible<X>::value)
    {
        if (dataSlotsAvailable == 0)
        {
            return;
        }
        for (int jj = bsize - 1; jj >= 0; --jj)
        {  // call destructors on the last block
            dataptr[dataSlotBack][jj].~X ();
        }
        // don't go into the front slot yet
        for (int ii = dataSlotBack - 1; ii >= dataSlotFront + 1; --ii)
        {
            for (int jj = blockSize - 1; jj >= 0; --jj)
            {  // call destructors on the middle blocks
                dataptr[ii][jj].~X ();
            }
            moveBlocktoAvailable (dataptr[ii]);
        }
        if (dataSlotFront < dataSlotBack)
        {
            moveBlocktoAvailable (dataptr[dataSlotBack]);

            for (int jj = blockSize - 1; jj > fsize; --jj)
            {  // call destructors on the first block
                dataptr[dataSlotFront][jj].~X ();
            }
        }
        csize = 0;
        auto nindex = dataSlotsAvailable / 2;
        dataptr[nindex] = dataptr[dataSlotFront];
        fsize = blockSize / 2;
        bsize = fsize + 1;
        dataSlotFront = nindex;
        dataSlotBack = nindex;
    }

    void shrink_to_fit ()
    {
        Allocator a;
        for (int ii = 0; ii <= freeIndex; ++ii)
        {
            a.deallocate (freeblocks[ii], blockSize);
        }
        freeIndex = 0;
    }

    /** get the last element*/
    X &back ()
    {
        return (bsize == 0) ? dataptr[dataSlotBack - 1][blockSize - 1] :
                              dataptr[dataSlotBack][bsize - 1];
    }
    /** const ref to last element*/
    const X &back () const
    {
        return (bsize == 0) ? dataptr[dataSlotBack - 1][blockSize - 1] :
                              dataptr[dataSlotBack][bsize - 1];
    }

    X &front ()
    {
        return (fsize == blockSize - 1) ? dataptr[dataSlotFront + 1][0] :
                                          dataptr[dataSlotFront][fsize + 1];
    }

    const X &front () const
    {
        return (fsize == blockSize - 1) ? dataptr[dataSlotFront + 1][0] :
                                          dataptr[dataSlotFront][fsize + 1];
    }

    X &operator[] (size_t n)
    {
        return dataptr[dataSlotFront + ((fsize + n + 1) >> N)]
                      [(fsize + n + 1) & cntmask];
    }

    const X &operator[] (size_t n) const
    {
        return dataptr[dataSlotFront + ((fsize + n + 1) >> N)]
                      [(fsize + n + 1) & cntmask];
    }

    size_t size () const { return csize; }

    bool empty () const { return (csize == 0); }

    iterator begin ()
    {
        if (fsize == blockSize - 1)
        {
            X **ptr = &(dataptr[dataSlotFront + 1]);
            return {ptr, 0};
        }
        else
        {
            X **ptr = &(dataptr[dataSlotFront]);
            return {ptr, fsize + 1};
        }
    }

    iterator end ()
    {
        if (bsize == blockSize)
        {
            X **ptr = &(dataptr[dataSlotBack + 1]);
            return {ptr, 0};
        }
        else
        {
            X **ptr = &(dataptr[dataSlotBack]);
            return {ptr, bsize};
        }
    }

    const_iterator begin () const
    {
        if (fsize == blockSize - 1)
        {
            X **ptr = &(dataptr[dataSlotFront + 1]);
            return {ptr, 0};
        }
        else
        {
            X **ptr = &(dataptr[dataSlotFront]);
            return {ptr, fsize + 1};
        }
    }

    const_iterator end () const
    {
        if (bsize == blockSize)
        {
            X **ptr = &(dataptr[dataSlotBack + 1]);
            return {ptr, 0};
        }
        else
        {
            X **ptr = &(dataptr[dataSlotBack]);
            return {ptr, bsize};
        }
    }

  private:
    void blockCheck ()
    {
        if (bsize >= blockSize)
        {
            if (0 == dataSlotsAvailable)
            {
                dataptr = new X *[64];
                dataSlotsAvailable = 64;
                dataSlotFront = dataSlotBack = 30;
                fsize = blockSize / 2 - 1;
                bsize = fsize + 1;
                dataptr[dataSlotBack] = getNewBlock ();
                return;
            }
            else if (dataSlotBack >= dataSlotsAvailable - 1)
            {
                if (dataSlotFront > 5)
                {  // shift the pointers
                    std::copy (dataptr + dataSlotFront,
                               dataptr + dataSlotBack + 1,
                               dataptr + dataSlotFront / 2);
                    auto diff = dataSlotBack - dataSlotFront;
                    dataSlotFront = dataSlotFront / 2;
                    dataSlotBack = dataSlotFront + diff;
                }
                else
                {
                    auto mem = new X *[dataSlotsAvailable * 2];
                    std::copy (dataptr, dataptr + dataSlotsAvailable, mem);
                    delete[] dataptr;
                    dataptr = mem;
                    dataSlotsAvailable *= 2;
                }
            }
            dataptr[++dataSlotBack] = getNewBlock ();
            bsize = 0;
        }
    }

    void blockCheckFront ()
    {
        if (fsize < 0)
        {
            if (0 == dataSlotsAvailable)
            {
                dataptr = new X *[64];
                dataSlotsAvailable = 64;
                dataSlotFront = dataSlotBack 30;
                fsize = blockSize / 2 - 1;
                bsize = fsize + 1;
                dataptr[dataSlotBack] = getNewBlock ();
                return;
            }
            else if (dataSlotFront == 0)
            {
                if (dataSlotBack < dataSlotsAvailable - 5)
                {
                }
                else
                {
                    auto mem = new X *[dataSlotsAvailable * 2];
                    std::copy (dataptr, dataptr + dataSlotBack + 1,
                               mem + dataSlotsAvailable);
                    delete[] dataptr;
                    dataptr = mem;
                    dataSlotFront = dataSlotsAvailable;
                    dataSlotsAvailable *= 2;
                }
            }
            dataptr[--dataSlotFront] = getNewBlock ();
            fsize = blockSize - 1;
        }
    }

    void moveBlocktoAvailable (X *ptr)
    {
        if (freeIndex >= freeSlotsAvailable)
        {
            if (0 == freeSlotsAvailable)
            {
                freeblocks = new X *[64];
                freeSlotsAvailable = 64;
            }
            else
            {
                auto mem = new X *[freeSlotsAvailable * 2];
                std::copy (freeblocks, freeblocks + freeSlotsAvailable, mem);
                delete[] freeblocks;
                freeblocks = mem;
                freeSlotsAvailable *= 2;
            }
        }
        freeblocks[freeIndex++] = ptr;
    }

    X *getNewBlock ()
    {
        if (freeIndex == 0)
        {
            Allocator a;
            return a.allocate (blockSize);
        }
        return freeblocks[--freeIndex];
    }

  private:
    size_t csize{0};  // 8
    X **dataptr{nullptr};  // 16
    int dataSlotsAvailable{0};  // 20
    int dataSlotBack{0};  // 24
    int dataSlotFront{0};  // 28
    int bsize{blockSize};  // 32
    int fsize{-1};  // 36
    int freeSlotsAvailable{0};  // 40
    int freeIndex{0};  // 48 +4 byte gap
    X **freeblocks{nullptr};  // 56
};

}  // namespace containers
}  // namespace gmlc
