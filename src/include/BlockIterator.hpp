#pragma once
#include <type_traits>

template <typename X, int BLOCKSIZE, typename OUTER>
class BlockIterator
{
  private:
    OUTER vec;
    X *ptr;
    int offset;

  public:
    using constref = typename const std::remove_const<X>::type;

    BlockIterator (OUTER &it, int startoffset) : vec (it), ptr (&((*it)[0])), offset (startoffset)
    {
        static_assert (std::is_same<std::remove_reference_t<decltype (*(*it))>, X>::value,
                       "OUTER *it must be dereferencable to a a type matching X");
    }

    std::enable_if_t<!std::is_const<X>::value, X> &operator* () { return *ptr; }
    constref &operator* () const { return *ptr; }
    std::enable_if_t<!std::is_const<X>::value, X> *operator-> () { return ptr; }
    constref *operator-> () const { return ptr; }

    operator bool () const { return (ptr != nullptr); }
    bool operator== (const BlockIterator &it) const { return ((vec == it.vec) && (offset == it.offset)); }
    bool operator!= (const BlockIterator &it) const { return ((vec != it.vec) || (offset != it.offset)); }

    BlockIterator &operator+= (const ptrdiff_t &movement)
    {
        ptr += movement;
        offset += movement;
        check ();
        return (*this);
    }
    BlockIterator &operator++ ()
    {
        ++ptr;
        ++offset;
        check ();
        return (*this);
    }
    BlockIterator operator++ (int)
    {
        auto temp (*this);
        ++(*this);
        return temp;
    }
    BlockIterator operator+ (const ptrdiff_t &movement) const
    {
        auto temp (*this);
        temp += movement;
        return temp;
    }
    BlockIterator &operator-= (const ptrdiff_t &movement)
    {
        ptr -= movement;
        offset -= movement;
        checkNeg ();
        return (*this);
    }
    BlockIterator &operator-- ()
    {
        --ptr;
        --offset;
        checkNeg ();
        return (*this);
    }
    BlockIterator operator-- (int)
    {
        auto temp (*this);
        --(*this);
        return temp;
    }
    BlockIterator operator- (const ptrdiff_t &movement) const
    {
        auto temp (*this);
        temp -= movement;
        return temp;
    }

  private:
    void check ()
    {
        if (offset >= BLOCKSIZE)
        {
            auto diff = offset - BLOCKSIZE;
            vec += 1 + (diff / BLOCKSIZE);
            offset = diff % BLOCKSIZE;
            ptr = &((*vec)[offset]);
        }
    }
    void checkNeg ()
    {
        if (offset < 0)
        {
            auto diff = -offset;
            vec -= (1 + (diff / BLOCKSIZE));
            offset = diff % BLOCKSIZE;
            ptr = &((*vec)[offset]);
        }
    }
};
