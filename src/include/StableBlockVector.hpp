#pragma once

#include <type_traits>
#include "BlockIterator.hpp"
#include <memory>

/** class to function as a vector in with push back and pop_back
But the memory is not contiguous and the elements are stable
no erase or insert
@tparam X the type to store in the vector
@tparam N the blocksize exponent store 2^N objects in an allocation block*/
template <typename X, unsigned int N, class Allocator = std::allocator<X>>
class StableBlockVector
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
	StableBlockVector() noexcept
	{

	}

	/** construct with a specified size*/
	explicit StableBlockVector(size_t startSize):csize(startSize),dataptr(new X*[std::max((startSize>>N)+1,64)]), dataSlotsAvailable(std::max((startSize >> N) + 1, 64)),dataSlotIndex(startSize >> N),bsize(startSize&cntmask)
	{
		Allocator a;
		if (startSize==0)
		{
			dataptr[0]= a.allocate(blockSize);
		}
		else
		{
			for (int ii=0;ii<=dataSlotIndex;++ii)
			{
				dataptr[ii] = a.allocate(blockSize);
			}
		}

	}
	StableBlockVector(const StableBlockVector &sbv) :StableBlockVector()
	{
		if (sbv.dataptr!=nullptr)
		{
			dataptr = new X*[sbv.dataSlotsAvailable];
			dataSlotsAvailable = sbv.dataSlotsAvailable;
			dataptr[0] = getNewBlock();
			assign(sbv.begin(), sbv.end());
		}
		else
		{
			bsize = blockSize;
		}
		
	}
	StableBlockVector(StableBlockVector &&sbv) noexcept:csize(sbv.csize),dataptr(sbv.dataptr),dataSlotsAvailable(sbv.dataSlotsAvailable),
		dataSlotIndex(sbv.dataSlotIndex),bsize(sbv.bsize),freeSlotsAvailable(sbv.freeSlotsAvailable),freeIndex(sbv.freeIndex),freeblocks(sbv.freeblocks)
	{
		sbv.freeblocks = nullptr;
		sbv.freeSlotsAvailable = 0;
		sbv.dataSlotsAvailable = 0;
		sbv.dataptr = nullptr;
	}

	StableBlockVector &operator=(const StableBlockVector &sbv)
	{
		clear();
		assign(sbv.begin(), sbv.end());
		return *this;
	}
	StableBlockVector &operator=(StableBlockVector &&sbv) noexcept
	{
		csize = sbv.csize;
		dataptr = sbv.dataptr;
		dataSlotsAvailable = sbv.dataSlotsAvailable;
		dataSlotIndex = sbv.dataSlotIndex;
		bsize = sbv.bsize;
		freeSlotsAvailable = sbv.freeSlotsAvailable;
		freeIndex = sbv.freeIndex;
		freeblocks = sbv.freeblocks;
			

			sbv.freeblocks = nullptr;
			sbv.freeSlotsAvailable = 0;
			sbv.dataSlotsAvailable = 0;
			sbv.dataptr = nullptr;

			return *this;
	}
	/** destructor*/
	~StableBlockVector()
	{
		Allocator a;
		for (int ii = 0; ii <= dataSlotIndex; ++ii)
		{
			a.deallocate(dataptr[ii],blockSize);
		}
		for (int ii = 0; ii < freeIndex; ++ii)
		{
			a.deallocate(freeblocks[ii], blockSize);
		}
		//delete can handle a nullptr
		delete[] freeblocks;
		delete[] dataptr;
	}

	template <class... Args>
	void emplace_back(Args&&... args)
	{
		blockCheck();
		dataptr[dataSlotIndex][bsize++] = X{std::forward<Args>(args)...};
		++csize;
	}

	void push_back(const X& val)
	{
		blockCheck();
		dataptr[dataSlotIndex][bsize++] = val;
		++csize;
	}
	void push_back(X&& val)
	{
		blockCheck();
		dataptr[dataSlotIndex][bsize++] = std::move(val);
		++csize;
	}

	void pop_back()
	{
		--csize;
		if (bsize>0)
		{
			--bsize;
		}
		else
		{
			moveBlocktoAvailable(dataptr[dataSlotIndex--]);
			bsize = blockSize - 1;
		}

	}

	template< class InputIt >
	void assign(InputIt first, InputIt last)
	{

	}

	template< class InputIt >
	void move_assign(InputIt first, InputIt last)
	{

	}

	void clear()
	{
		//start at 1 to leave the first slot in use like the constructor
		for (int ii = 1; ii <= dataSlotIndex; ++ii)
		{
			moveBlocktoAvailable(dataptr[ii]);
		}
		csize = 0;
		bsize = 0;
		dataSlotIndex = 0;
	}
	void shrink_to_fit()
	{
		Allocator a;
		for (int ii = 0; ii <= freeIndex; ++ii)
		{
			a.deallocate(freeblocks[ii], blockSize);
		}
		freeIndex = 0;
	}
	/** get the last element*/
	X &back()
	{
		return (bsize == 0) ? dataptr[(csize >> N)-1][blockSize - 1] : dataptr[dataSlotIndex][bsize - 1];
	}
	/** const ref to last element*/
	const X &back() const
	{
		return (bsize == 0) ? dataptr[(csize >> N)-1][blockSize - 1] : dataptr[dataSlotIndex][bsize - 1];
	}

	X &front()
	{
		return dataptr[0][0];
	}

	const X &front() const
	{
		return dataptr[0][0];
	}

	X& operator[] (size_t n)
	{
		return *dataptr[n >> N][n&cntmask];
	}

	const X& operator[] (size_t n) const
	{
		return *dataptr[n >> N][n&cntmask];
	}

	size_t size() const { return csize; }

	bool empty() const { return (csize == 0); }
	/** define an iterator*/
	

	iterator begin()
	{
		return { dataptr, 0};
	}

	iterator end()
	{
		X** ptr = &(dataptr[dataSlotIndex]);
		return { ptr,bsize };
	}

	const_iterator begin() const
	{
		return { dataptr, 0 };
	}

	const_iterator end() const
	{
		return { &(dataptr[dataSlotIndex]),bsize };
	}

private:
	void blockCheck()
	{
		if (bsize >= blockSize)
		{
			if (0==dataSlotsAvailable)
			{
				dataptr = new X*[64];
				dataSlotsAvailable = 64;
			}
			else if (dataSlotIndex >= dataSlotsAvailable - 1)
			{
				auto mem = new X*[dataSlotsAvailable * 2];
				std::copy(dataptr, dataptr+dataSlotsAvailable, mem);
				delete[] dataptr;
				dataptr = mem;
				dataSlotsAvailable *= 2;
			}
			dataptr[++dataSlotIndex] = getNewBlock();
			bsize = 0;
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
				std::copy(freeblocks, freeblocks + freeSlotsAvailable, mem);
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
			Allocator a;
			return a.allocate(blockSize);
		}
		return freeblocks[--freeIndex];
	}
private:
	size_t csize{0};  //8
	X **dataptr{nullptr};  //16
	int dataSlotsAvailable{0}; //20
	int dataSlotIndex{0}; //24
	int bsize{blockSize}; //28
	int freeSlotsAvailable{0}; //32
	int freeIndex{0}; //36 +4 byte gap
	X **freeblocks{nullptr}; //48
	
};
