/*
Copyright (c) 2017-2026,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved.

SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <optional>

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <type_traits>
#include <utility>

namespace gmlc::containers {

/** NOTES:: PT Went with unlocking after signaling on the basis of this page
http://www.domaigne.com/blog/computing/condvars-signal-with-mutex-locked-or-not/
will check performance at a later time
*/
/** class implementing a airlock
@details this class is used to transfer an object from a thread safe context to
a single thread so it can be accessed without locks
*/
template<
    typename T,
    class MUTEX = std::mutex,
    class COND = std::condition_variable>
class AirLock {
  public:
    /** default constructor */
    AirLock() = default;
    ~AirLock() { close(); }
    /** try to load the airlock
@return true if successful, false if not*/
    template<class Z>
    bool try_load(Z&& val)
    {  // all modifications to loaded should be inside the mutex otherwise
       // this will contain race conditions
        if (!loaded.load(std::memory_order_acquire) &&
            !closed.load(std::memory_order_acquire)) {
            std::lock_guard<MUTEX> lock(door);
            if (!loaded.load(std::memory_order_acquire) &&
                !closed.load(std::memory_order_acquire)) {
                data = std::forward<Z>(val);
                loaded.store(true, std::memory_order_release);
                return true;
            }
        }
        return false;
    }

    /** load the airlock,
    @details the call will block until the airlock is ready to be loaded
    */
    template<class Z>
    bool load(Z&& val)
    {
        std::unique_lock<MUTEX> lock(door);
        if (closed.load(std::memory_order_acquire)) {
            return false;
        }
        if (!loaded.load(std::memory_order_acquire)) {
            data = std::forward<Z>(val);
            loaded.store(true, std::memory_order_release);
            return true;
        } else {
            while (loaded.load(std::memory_order_acquire) &&
                   !closed.load(std::memory_order_acquire)) {
                condition.wait(lock);
            }
            if (closed.load(std::memory_order_acquire)) {
                return false;
            }
            data = std::forward<Z>(val);
            loaded.store(true, std::memory_order_release);
            return true;
        }
    }

    /** unload the airlock,
@return the value is returned in an optional which needs to be checked if it
contains a value
*/
    std::optional<T> try_unload()
    {
        if (loaded.load(std::memory_order_acquire)) {
            std::lock_guard<MUTEX> lock(door);
            // can use relaxed since we are behind a mutex
            if (loaded.load(std::memory_order_acquire)) {
                std::optional<T> val{std::move(data)};
                loaded.store(false, std::memory_order_release);
                condition.notify_one();
                return val;
            }
        }
        return std::nullopt;
    }
    /** close the airlock to further loads and wake any waiting loader*/
    void close()
    {
        bool expected = false;
        if (closed.compare_exchange_strong(expected, true)) {
            condition.notify_all();
        }
    }
    /** check if the airlock is loaded
@details this may or may  not mean anything depending on usage
it is correct but may be incorrect immediately after the call
*/
    bool isLoaded() const { return loaded.load(std::memory_order_acquire); }
    /** check if the airlock is closed to further loads */
    bool isClosed() const { return closed.load(std::memory_order_acquire); }

  private:
    std::atomic_bool loaded{
        false};  //!< flag if the airlock is loaded with cargo
    std::atomic_bool closed{
        false};  //!< flag indicating the airlock is closed to new cargo
    MUTEX door;  //!< check if one of the doors to the airlock is open
    T data{};  //!< the data to be stored in the airlock
    COND condition;  //!< condition variable for notification of new data
};

}  // namespace gmlc::containers
