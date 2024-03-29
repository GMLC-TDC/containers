/*
Copyright (c) 2017-2022,
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
#include <queue>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace gmlc {
namespace containers {
    /** class implementing a blocking queue with a priority channel
@details this class uses locks one for push and pull it can exhibit longer
blocking times if the internal operations require a swap, however in high usage
the two locks will reduce contention in most cases.
*/
    template<
        typename T,
        class MUTEX = std::mutex,
        class COND = std::condition_variable>
    class BlockingPriorityQueue {
      private:
        mutable MUTEX m_pushLock;  //!< lock for operations on the pushElements
                                   //!< vector
        mutable MUTEX m_pullLock;  //!< lock for elements on the pullLock vector
        std::vector<T> pushElements;  //!< vector of elements being added
        std::vector<T> pullElements;  //!< vector of elements waiting extraction
        std::atomic<bool> queueEmptyFlag{
            true};  //!< flag indicating the queue is empty
        std::queue<T> priorityQueue;  //!< the priority channel
        // the condition variable should be keyed of the pullLock
        COND condition;  //!< condition variable for notification of new data
      public:
        /** default constructor*/
        BlockingPriorityQueue() = default;

        /** clear the queue*/
        void clear()
        {
            std::lock_guard<MUTEX> pullLock(m_pullLock);  // first pullLock
            std::lock_guard<MUTEX> pushLock(m_pushLock);  // second pushLock
            pullElements.clear();
            pushElements.clear();
            while (!priorityQueue.empty()) {
                priorityQueue.pop();
            }
            queueEmptyFlag = true;
        }

        ~BlockingPriorityQueue() { clear(); }
        /** constructor with the capacity numbers
    @details there are two internal vectors that alternate
    so the actual reserve is 2x the capacity numbers in two different vectors
    @param capacity the initial reserve capacity for the arrays
    */
        explicit BlockingPriorityQueue(size_t capacity)
        {  // don't need to lock since we aren't out of the constructor yet
            pushElements.reserve(capacity);
            pullElements.reserve(capacity);
        }
        /** enable the move constructor not the copy constructor*/
        BlockingPriorityQueue(BlockingPriorityQueue&& bq) noexcept :
            pushElements(std::move(bq.pushElements)),
            pullElements(std::move(bq.pullElements)),
            priorityQueue(std::move(bq.priorityQueue))
        {
            queueEmptyFlag = (pullElements.empty() && priorityQueue.empty());
        }

        /** enable the move assignment not the copy assignment*/
        BlockingPriorityQueue& operator=(BlockingPriorityQueue&& sq) noexcept
        {
            std::lock_guard<MUTEX> pullLock(m_pullLock);  // first pullLock
            std::lock_guard<MUTEX> pushLock(m_pushLock);  // second pushLock
            pushElements = std::move(sq.pushElements);
            pullElements = std::move(sq.pullElements);
            priorityQueue = std::move(sq.priorityQueue);
            queueEmptyFlag = (pullElements.empty() && priorityQueue.empty());
            return *this;
        }
        /** DISABLE_COPY_AND_ASSIGN */
        BlockingPriorityQueue(const BlockingPriorityQueue&) = delete;
        BlockingPriorityQueue& operator=(const BlockingPriorityQueue&) = delete;

        /** set the capacity of the queue
    actually double the requested the size will be reserved due to the use of
    two vectors internally
    @param capacity  the capacity to reserve
    */
        void reserve(size_t capacity)
        {
            std::lock_guard<MUTEX> pullLock(m_pullLock);  // first pullLock
            std::lock_guard<MUTEX> pushLock(m_pushLock);  // second pushLock
            pullElements.reserve(capacity);
            pushElements.reserve(capacity);
        }

        /** push an element onto the queue
    val the value to push on the queue
    */
        template<class Z>
        void push(Z&& val)  // forwarding reference
        {
            std::unique_lock<MUTEX> pushLock(
                m_pushLock);  // only one lock on this branch
            if (pushElements.empty()) {
                bool expEmpty = true;
                if (queueEmptyFlag.compare_exchange_strong(expEmpty, false)) {
                    // release the push lock so we don't get a potential
                    // deadlock condition
                    pushLock.unlock();
                    std::unique_lock<MUTEX> pullLock(
                        m_pullLock);  // first pullLock
                    queueEmptyFlag = false;
                    if (pullElements.empty()) {
                        pullElements.push_back(std::forward<Z>(val));
                        // pullLock.unlock ();
                    } else {
                        pushLock.lock();
                        pushElements.push_back(std::forward<Z>(val));
                    }
                    condition.notify_all();
                    return;
                } else {
                    pushElements.push_back(std::forward<Z>(val));
                    expEmpty = true;
                    if (queueEmptyFlag.compare_exchange_strong(
                            expEmpty, false)) {
                        condition.notify_all();
                    }
                    return;
                }
            }
            pushElements.push_back(std::forward<Z>(val));
        }

        /** push an element onto the queue
    val the value to push on the queue
    */
        template<class Z>
        void pushPriority(Z&& val)  // forwarding reference
        {
            bool expEmpty = true;
            if (queueEmptyFlag.compare_exchange_strong(expEmpty, false)) {
                std::unique_lock<MUTEX> pullLock(m_pullLock);  // first pullLock
                queueEmptyFlag = false;  // need to set the flag again just in
                                         // case after we get the lock
                priorityQueue.push(std::forward<Z>(val));
                // pullLock.unlock ();
                condition.notify_all();
            } else {
                std::unique_lock<MUTEX> pullLock(m_pullLock);
                priorityQueue.push(std::forward<Z>(val));
                expEmpty = true;
                if (queueEmptyFlag.compare_exchange_strong(expEmpty, false)) {
                    condition.notify_all();
                }
            }
        }

        /** construct on object in place on the queue */
        template<class... Args>
        void emplace(Args&&... args)
        {
            std::unique_lock<MUTEX> pushLock(
                m_pushLock);  // only one lock on this branch
            if (pushElements.empty()) {
                bool expEmpty = true;
                if (queueEmptyFlag.compare_exchange_strong(expEmpty, false)) {
                    // release the push lock so we don't get a potential
                    // deadlock condition
                    pushLock.unlock();
                    std::unique_lock<MUTEX> pullLock(
                        m_pullLock);  // first pullLock
                    queueEmptyFlag = false;  // need to set the flag again after
                                             // we get the lock
                    if (pullElements.empty()) {
                        pullElements.emplace_back(std::forward<Args>(args)...);
                    } else {
                        pushLock.lock();
                        pushElements.emplace_back(std::forward<Args>(args)...);
                    }

                    condition.notify_all();
                    return;
                } else {
                    pushElements.emplace_back(std::forward<Args>(args)...);
                    expEmpty = true;
                    if (queueEmptyFlag.compare_exchange_strong(
                            expEmpty, false)) {
                        condition.notify_all();
                    }
                    return;
                }
            }
            pushElements.emplace_back(std::forward<Args>(args)...);
        }

        /** emplace an element onto the priority queue
    val the value to push on the queue
    */
        template<class... Args>
        void emplacePriority(Args&&... args)
        {
            bool expEmpty = true;
            if (queueEmptyFlag.compare_exchange_strong(expEmpty, false)) {
                std::unique_lock<MUTEX> pullLock(m_pullLock);  // first pullLock
                queueEmptyFlag = false;  // need to set the flag again just in
                                         // case after we get the lock
                priorityQueue.emplace(std::forward<Args>(args)...);
                // pullLock.unlock ();
                condition.notify_all();
            } else {
                std::unique_lock<MUTEX> pullLock(m_pullLock);
                priorityQueue.emplace(std::forward<Args>(args)...);
                expEmpty = true;
                if (queueEmptyFlag.compare_exchange_strong(expEmpty, false)) {
                    condition.notify_all();
                }
            }
        }
        /** try to peek at an object without popping it from the stack
    @details only available for copy assignable objects
    @return an optional object with an object of type T if available
    */
        template<typename = std::enable_if<std::is_copy_assignable<T>::value>>
        std::optional<T> try_peek() const
        {
            std::lock_guard<MUTEX> lock(m_pullLock);
            if (!priorityQueue.empty()) {
                return priorityQueue.front();
            }
            if (pullElements.empty()) {
                return std::nullopt;
            }

            auto t = pullElements.back();
            return t;
        }

        /** try to pop an object from the queue
    @return an optional containing the value if successful the optional will be
    empty if there is no element in the queue
    */
        std::optional<T> try_pop();

        /** blocking call to wait on an object from the stack*/
        T pop()
        {
            T actval;
            auto val = try_pop();
            while (!val) {
                std::unique_lock<MUTEX> pullLock(
                    m_pullLock);  // get the lock then wait
                if (!priorityQueue.empty()) {
                    actval = std::move(priorityQueue.front());
                    priorityQueue.pop();
                    return actval;
                }
                if (!pullElements.empty())  // make sure we are actually empty;
                {
                    actval = std::move(pullElements.back());
                    pullElements.pop_back();
                    return actval;
                }
                condition.wait(pullLock);  // now wait
                if (!priorityQueue.empty()) {
                    actval = std::move(priorityQueue.front());
                    priorityQueue.pop();
                    return actval;
                }
                if (!pullElements.empty())  // check for spurious wake-ups
                {
                    actval = std::move(pullElements.back());
                    pullElements.pop_back();
                    return actval;
                }
                pullLock.unlock();
                val = try_pop();
            }
            // move the value out of the optional
            actval = std::move(*val);
            return actval;
        }

        /** blocking call to wait on an object from the stack with timeout*/
        template<typename TIME>
        std::optional<T> pop(TIME timeout)
        {
            auto val = try_pop();
            while (!val) {
                std::unique_lock<MUTEX> pullLock(
                    m_pullLock);  // get the lock then wait
                if (!priorityQueue.empty()) {
                    val = std::move(priorityQueue.front());
                    priorityQueue.pop();
                    break;
                }
                if (!pullElements.empty())  // make sure we are actually empty;
                {
                    val = std::move(pullElements.back());
                    pullElements.pop_back();
                    break;
                }
                auto res = condition.wait_for(pullLock, timeout);  // now wait

                if (!priorityQueue.empty()) {
                    val = std::move(priorityQueue.front());
                    priorityQueue.pop();
                    break;
                }
                if (!pullElements.empty())  // check for spurious wake-ups
                {
                    val = std::move(pullElements.back());
                    pullElements.pop_back();
                    break;
                }
                pullLock.unlock();
                val = try_pop();
                if (res !=
                    std::cv_status::no_timeout)  // std::cv_status::no_timeout
                {
                    break;
                }
            }
            // move the value out of the optional
            return val;
        }

        /** blocking call that will call the specified functor
    if the queue is empty
    @param callOnWaitFunction an nullary functor that will be called if the
    initial query does not return a value
    @details  after calling the function the call will check again and if still
    empty will block and wait.
    */
        template<typename Functor>
        T popOrCall(Functor callOnWaitFunction)
        {
            auto val = try_pop();
            while (!val) {
                // may be spurious so make sure actually have a value
                callOnWaitFunction();
                std::unique_lock<MUTEX> pullLock(m_pullLock);  // first pullLock
                if (!priorityQueue.empty()) {
                    auto actval = std::move(priorityQueue.front());
                    priorityQueue.pop();
                    return actval;
                }
                if (!pullElements.empty()) {
                    // the callback may fill the queue or it may have been
                    // filled in the meantime
                    auto actval = std::move(pullElements.back());
                    pullElements.pop_back();
                    return actval;
                }
                condition.wait(pullLock);
                // need to check again to handle spurious wake-up
                if (!priorityQueue.empty()) {
                    auto actval = std::move(priorityQueue.front());
                    priorityQueue.pop();
                    return actval;
                }
                if (!pullElements.empty()) {
                    auto actval = std::move(pullElements.back());
                    pullElements.pop_back();
                    return actval;
                }
                pullLock.unlock();
                val = try_pop();
            }
            return std::move(*val);
        }

        /** check whether there are any elements in the queue
because this is meant for multi-threaded applications this may or may not have
any meaning depending on the number of consumers
*/
        bool empty() const;

      private:
        /** If pullElements is empty check push and swap and reverse if needed
  assumes pullLock is active and pushLock is not
  */
        void checkPullAndSwap()
        {
            if (pullElements.empty()) {
                std::unique_lock<MUTEX> pushLock(
                    m_pushLock);  // second pushLock
                if (!pushElements.empty()) {  // this is the potential for slow
                                              // operations
                    std::swap(pushElements, pullElements);
                    // we can free the push function to accept more elements
                    // after the swap call;
                    pushLock.unlock();
                    std::reverse(pullElements.begin(), pullElements.end());
                } else {
                    queueEmptyFlag = true;
                }
            }
        }
    };

    template<typename T, class MUTEX, class COND>
    std::optional<T> BlockingPriorityQueue<T, MUTEX, COND>::try_pop()
    {
        std::lock_guard<MUTEX> pullLock(m_pullLock);  // first pullLock
        if (!priorityQueue.empty()) {
            std::optional<T> val(std::move(priorityQueue.front()));
            priorityQueue.pop();
            return val;
        }
        checkPullAndSwap();
        if (pullElements.empty()) {
            return std::nullopt;
        }
        // do it this way to allow movable only types
        std::optional<T> val(std::move(pullElements.back()));
        pullElements.pop_back();
        checkPullAndSwap();
        return val;
    }

    template<typename T, class MUTEX, class COND>
    bool BlockingPriorityQueue<T, MUTEX, COND>::empty() const
    {
        return queueEmptyFlag.load();
    }

}  // namespace containers
}  // namespace gmlc
