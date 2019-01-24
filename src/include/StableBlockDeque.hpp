#pragma once

#include <type_traits>
#include "BlockIterator.hpp"

/** class to function similarly to a deque  with push/ppp back and front
But the memory is not contiguous and the elements are stable
no erase or insert
@tparam X the type to store in the vector
@tparam N the blocksize exponent store 2^N objects in an allocation block*/

template <typename X, unsigned int N>
class StableBlockDeque
{
	static_assert (N < 32, "N should be between 0 and 31 data will allocated in block 2^N");
	static_assert (std::is_default_constructible<X>::value, " used type must be default constructible");
public:
	static constexpr unsigned int blockSize{ 1u << N };
	using iterator = BlockIterator<X, (1u << N), X **>;
	using const_iterator = BlockIterator<const X, (1u << N), X *const *>;
private:
	static constexpr unsigned int cntmask{ blockSize - 1 };
public:
	/** constructor*/
	StableBlockDeque() :dataptr(new X*[64]), dataSlotsAvailable(64),dataSlotBack{30},dataSlotFront{30}
	{
		dataptr[30]=new X[blockSize];
		bsize = fsize = blockSize / 2;
	}
	/** construct with a specified size*/
	StableBlockDeque(size_t startSize):csize(startSize), dataptr(new X*[std::max((startSize >> N) + 1, 64)]),
		dataSlotsAvailable(std::max((startSize >> N) + 1, 64)),
		dataSlotBack(startSize >> N),
		bsize(startSize&cntmask)
	{
		if (startSize == 0)
		{
			dataptr[30] = new X[blockSize];
			bsize = fsize = blockSize / 2;
		}
		else
		{
			for (int ii = dataSlotFront; ii <= dataSlotBack; ++ii)
			{
				dataptr[ii] = new X[blockSize];
			}
		}

	}
	/** destructor*/
	~StableBlockDeque()
	{
		for (int ii = dataSlotFront; ii <= dataSlotBack; ++ii)
		{
			delete[] dataptr[ii];
		}
		for (int ii = 0; ii < freeIndex; ++ii)
		{
			delete[] freeblocks[ii];
		}
		delete[] freeblocks;
		delete[] dataptr;
	}

	template <class... Args>
	void emplace_back(Args&&... args)
	{
		blockCheck();
		dataptr[dataSlotBack][bsize++] = X{ std::forward<Args>(args)... };
		++csize;
	}

	void push_back(const X& val)
	{
		blockCheck();
		dataptr[dataSlotBack][bsize++] = val;
		++csize;
	}
	void push_back(X&& val)
	{
		blockCheck();
		dataptr[dataSlotBack][bsize++] = std::move(val);
		++csize;
	}

	void pop_back()
	{
		--csize;
		if (bsize > 0)
		{
			--bsize;
		}
		else
		{
			moveBlocktoAvailable(dataptr[dataSlotBack--]);
			bsize = blockSize - 1;
		}

	}

	template <class... Args>
	void emplace_front(Args&&... args)
	{
		blockCheckFront();
		dataptr[dataSlotFront][fsize--] = X{ std::forward<Args>(args)... };
		++csize;
	}

	void push_front(const X& val)
	{
		blockCheckFront();
		dataptr[dataSlotFront][fsize--] = val;
		++csize;
	}
	void push_front(X&& val)
	{
		blockCheckFront();
		dataptr[dataSlotFront][fsize--] = std::move(val);
		++csize;
	}

	void pop_front()
	{
		--csize;
		if (fsize != blockSize-1)
		{
			++fsize;
		}
		else
		{
			moveBlocktoAvailable(dataptr[dataSlotFront++]);
			fsize = 0;
		}

	}

	void shrink_to_fit()
	{
		for (int ii = 0; ii <= freeIndex; ++ii)
		{
			delete[] freeblocks[ii];
		}
		freeIndex = 0;
	}

	/** get the last element*/
	X &back()
	{
		return (bsize == 0) ? dataptr[dataSlotBack - 1][blockSize - 1] : dataptr[dataSlotBack][bsize - 1];
	}
	/** const ref to last element*/
	const X &back() const
	{
		return (bsize == 0) ? dataptr[dataSlotBack - 1][blockSize - 1] : dataptr[dataSlotBack][bsize - 1];
	}

	X &front()
	{
		return (fsize == blockSize-1) ? dataptr[dataSlotFront+1][0] : dataptr[dataSlotFront][fsize + 1];
	}

	const X &front() const
	{
		return (fsize == blockSize - 1) ? dataptr[dataSlotFront + 1][0] : dataptr[dataSlotFront][fsize + 1];
	}

	X& operator[] (size_t n)
	{
		return *dataptr[dataSlotFront+((fsize+n+1) >> N)][(fsize+n+1)&cntmask];
	}

	const X& operator[] (size_t n) const
	{
		return *dataptr[dataSlotFront + ((fsize + n + 1) >> N)][(fsize + n + 1)&cntmask];
	}

	size_t size() const { return csize; }

	bool empty() const { return (csize == 0); }

	iterator begin()
	{
		return { &(dataptr[dataSlotBack]),bsize };
	}

	iterator end()
	{
		return { &(dataptr[dataSlotBack]),bsize };
	}

	const_iterator begin() const
	{
		return { &(dataptr[dataSlotBack]),bsize };
	}

	const_iterator end() const
	{
		return { &(dataptr[dataSlotBack]),bsize };
	}

private:
	void blockCheck()
	{
		if (bsize >= blockSize)
		{
			if (dataSlotBack >= dataSlotsAvailable-1)
			{
				if (dataSlotFront>5)
				{ //shift the pointers
					std::copy(dataptr+dataSlotFront, dataptr + dataSlotBack+1, dataptr+dataSlotFront/2);
					auto diff = dataSlotBack - dataSlotFront;
					dataSlotFront = dataSlotFront / 2;
					dataSlotBack = dataSlotFront + diff;
				}
				else
				{
					auto mem = new X*[dataSlotsAvailable * 2];
					std::copy(dataptr, dataptr + dataSlotsAvailable, mem);
					delete[] dataptr;
					dataptr = mem;
					dataSlotsAvailable *= 2;
				}
				
			}
			dataptr[++dataSlotBack] = getNewBlock();
			bsize = 0;
		}
	}

	void blockCheckFront()
	{
		if (fsize < 0)
		{
			if (dataSlotFront == 0)
			{
				if (dataSlotBack<dataSlotsAvailable-5)
				{
				}
				else
				{
					auto mem = new X*[dataSlotsAvailable * 2];
					std::copy(dataptr, dataptr + dataSlotBack + 1, mem + dataSlotsAvailable);
					delete[] dataptr;
					dataptr = mem;
					dataSlotFront = dataSlotsAvailable;
					dataSlotsAvailable *= 2;
				}
				
			}
			dataptr[--dataSlotFront] = getNewBlock();
			fsize = blockSize-1;
		}
	}

	void moveBlocktoAvailable(X *ptr)
	{
		if (freeIndex >= freeSlotsAvailable)
		{
			if (0 == freeSlotsAvailable)
			{
				freeblocks = new X*[64];
				freeSlotsAvailable = 64;
			}
			else
			{
				auto mem = new X*[freeSlotsAvailable * 2];
				std::copy( freeblocks,freeblocks+freeSlotsAvailable, mem);
				delete[] freeblocks;
				freeblocks = mem;
				freeSlotsAvailable *= 2;
			}
		}
		freeblocks[freeIndex++] = ptr;
	}

	X *getNewBlock()
	{
		if (freeIndex == 0)
		{
			return new X[blockSize];
		}
		return freeblocks[--freeIndex];
	}
private:
	size_t csize = 0;  //8
	X **dataptr;  //16
	int dataSlotsAvailable = 0; //20
	int dataSlotBack = 0; //24
	int dataSlotFront = 0; //28
	int bsize = 0; //32
	int fsize = 0; //36
	int freeSlotsAvailable = 0; //40
	int freeIndex = 0; //48 +4 byte gap
	X **freeblocks; //56
};
