/*
 * LLNS Copyright Start
 * Copyright (c) 2014-2018, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 */

/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved.

SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include "SimpleQueue.hpp"

#include <future>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

namespace gmlc {
namespace containers {
    /** basic work block abstract class*/
    class BasicWorkBlock {
      public:
        BasicWorkBlock() noexcept {}
        virtual ~BasicWorkBlock() = default;
        /** run the work block*/
        virtual void execute() = 0;
        /** check if the work is finished
    @return true if the work has been done false otherwise
    */
        virtual bool isFinished() const = 0;
    };

    /** a dummy work block that does nothing*/
    class NullWorkBlock : public BasicWorkBlock {
      public:
        NullWorkBlock() noexcept {}
        virtual void execute() override {}
        virtual bool isFinished() const override { return true; }
    };

    /** implementation of a workBlock class
The class takes as an input object of some kind function, std::function, lambda
that can be executed, functionoid, or something that implements operator()
*/
    template<typename retType>
    class WorkBlock : public BasicWorkBlock {
      public:
        WorkBlock() { reset(); }
        // copy constructor intentionally omitted*/
        /** move constructor*/
        WorkBlock(WorkBlock&& wb) = default;
        /** constructor from a packaged task*/
        WorkBlock(std::packaged_task<retType()>&& newTask) :
            task(std::move(newTask)), loaded(true)
        {
            reset();
        }
        /** construct from a some sort of functional object*/
        template<typename Func>  // forwarding reference
        WorkBlock(Func&& newWork) :
            task(std::forward<Func>(newWork)), loaded(true)
        {
            static_assert(
                std::is_same<decltype(newWork()), retType>::value,
                "work does not match type");
            reset();
        }
        /** move assignment*/
        WorkBlock& operator=(WorkBlock&& wb) = default;
        /** execute the work block*/
        virtual void execute() override
        {
            if (!finished) {
                if (loaded) {
                    task();
                }
                finished = true;
            }
        }
        /** get the return value,  will block until the task is finished*/
        retType getReturnVal() const { return future_ret.get(); }
        /** update the work function
    @param[in] newWork the work to do*/
        template<typename Func>
        void updateWorkFunction(Func&& newWork)  // forwarding reference
        {
            static_assert(
                std::is_same<decltype(newWork()), retType>::value,
                "work does not match type");
            loaded = false;
            task = std::packaged_task<retType()>(std::forward<Func>(newWork));
            reset();
            loaded = true;
        }
        /** update the task with a new packaged_task
    @param[in] newTask the packaged task with the correct return type*/
        void updateTask(std::packaged_task<retType()>&& newTask)
        {
            loaded = false;
            task = std::move(newTask);
            reset();
            loaded = true;
        }
        /** check if the task is finished*/
        bool isFinished() const override { return finished; }
        /** wait until the work is done*/
        void wait() { future_ret.wait(); }
        /** reset the work so it can run again*/
        void reset()
        {
            if (loaded) {
                task.reset();
            }
            finished = false;
            future_ret = task.get_future();
        };
        /** get the shared future object*/
        std::shared_future<retType> get_future() { return future_ret; }

      private:
        std::packaged_task<retType()> task;  //!< the task to do
        std::shared_future<retType> future_ret;  //!< shared future object
        bool finished;  //!< flag indicating the work has been done
        bool loaded = false;  //!< flag indicating that the task is loaded
    };

    /** implementation of a workBlock class with void return type
The class takes as an input object of some kind function, std::function, lambda
that can be executed
*/
    template<>
    class WorkBlock<void> : public BasicWorkBlock {
      public:
        WorkBlock() { reset(); }
        WorkBlock(std::packaged_task<void()>&& newTask) :
            task(std::move(newTask)), loaded(true)
        {
            reset();
        }

        template<typename Func>
        WorkBlock(Func&& newWork) :
            task(std::forward<Func>(newWork)), loaded(true)
        {
            static_assert(
                std::is_same<decltype(newWork()), void>::value,
                "work does not match type");
            reset();
        }

        WorkBlock(WorkBlock&& wb) = default;
        WorkBlock& operator=(WorkBlock&& wb) = default;
        virtual void execute() override
        {
            if (!finished) {
                if (loaded) {
                    task();
                }
                finished = true;
            }
        };
        void getReturnVal() const { future_ret.wait(); }

        template<typename Func>
        void updateWorkFunction(Func&& newWork)
        {
            static_assert(
                std::is_same<decltype(newWork()), void>::value,
                "work does not match type");
            loaded = false;
            task = std::packaged_task<void()>(std::forward<Func>(newWork));
            reset();
            loaded = true;
        }
        void updateTask(std::packaged_task<void()>&& newTask)
        {
            loaded = false;
            task = std::move(newTask);
            reset();
            loaded = true;
        }
        bool isFinished() const override { return finished; };
        void wait() const { future_ret.wait(); }
        void reset()
        {
            if (loaded) {
                task.reset();
            }
            finished = false;
            future_ret = task.get_future();
        };
        std::shared_future<void> get_future() { return future_ret; }

      private:
        std::packaged_task<void()> task;
        std::shared_future<void> future_ret;
        bool finished;
        bool loaded = false;
    };

    /** make a unique pointer to a workBlock object from a functional object
@param[in] fptr a std::function, std::bind, functionoid, lambda function or
anything else that could be called with operator()
@return a unique pointer to a work block function
*/
    template<typename X>
    auto make_workBlock(X&& fptr)
    {
        return std::make_unique<WorkBlock<decltype(fptr())>>(
            std::forward<X>(fptr));
    }
    /** make a shared pointer to a workBlock object from a functional object
@param[in] fptr a std::function, std::bind, functionoid, lambda function or
anything else that could be called with operator()
@return a shared pointer to a work block function
*/
    template<typename X>
    auto make_shared_workBlock(X&& fptr)
    {
        return std::make_shared<WorkBlock<decltype(fptr())>>(
            std::forward<X>(fptr));
    }

    /** make a unique pointer to a workBlock object from a packaged task object
@param[in] task a packaged task containing something to do
@return a unique pointer to a work block function
*/
    template<typename X>
    auto make_workBlock(std::packaged_task<X()>&& task)
    {
        return std::make_unique<WorkBlock<X>>(std::move(task));
    }

    /** make a shared pointer to a workBlock object from a packaged task object
@param[in] task a packaged task containing something to do
@return a unique pointer to a work block function
*/
    template<typename X>
    auto make_shared_workBlock(std::packaged_task<X()>&& task)
    {
        return std::make_shared<WorkBlock<X>>(std::move(task));
    }

    /** the default ratio between med and low priority tasks*/
    constexpr int defaultPriorityRatio{4};

    /** class defining a work queuing system
implemented with 3 priority levels high medium and low
high is executed as a soon as possible in order
medium is executed with X times more frequently than low with X being defined by
the priority ratio
*/
    class WorkQueue {
      public:
        /** enumeration defining the work block priority*/
        enum class WorkPriority {
            medium,  //!< do the work with a specific ratio of priority to low
            //!< priority tasks
            low,  //!< low priority do the work every once in a while determined
                  //!< by
            //!< the priority ratio
            high,  //!< do the work as soon as possible
            required,  //!< high priority work job that always is scheduled
        };

        /** construct a queue
@param[in] threadCount  the number of threads in the queue (<0 for default
value)
*/
        explicit WorkQueue(int threadCount) :
            numWorkers(
                (threadCount >= 0) ?
                    threadCount :
                    static_cast<int>(std::thread::hardware_concurrency()) + 1)
        {
            if (numWorkers != 0) {
                threadpool.resize(numWorkers);
                for (int kk = 0; kk < numWorkers; ++kk) {
                    threadpool[kk] = std::thread(&WorkQueue::workerLoop, this);
                }
            }
        }

        /**
         * class destructor to close out all threads
         */
        ~WorkQueue()
        {
            if (!halt) {
                closeWorkerQueue();
            }
        }

        /** get the number of workers
    @return int with the current worker count
    */
        int getWorkerCount() { return (!halt) ? numWorkers : 0; }
        /** destroy the WorkQueue*/
        void closeWorkerQueue()
        {
            std::unique_lock<std::mutex> lv(queueLock);
            if (!halt.load()) {
                halt.store(true);
                lv.unlock();
                auto dummyWork = std::make_shared<NullWorkBlock>();

                workToDoHigh.clear();
                workToDoMed.clear();
                workToDoLow.clear();

                queueCondition.notify_all();
                for (int ii = 0; ii < numWorkers; ++ii) {
                    addWorkBlock(dummyWork, WorkPriority::required);
                }
                queueCondition.notify_all();
            }
            for (auto& thrd : threadpool) {
                if (thrd.joinable()) {
                    thrd.join();
                }
            }
        }

        /** add a block of work to the WorkQueue
    @param[in] newWork  the block of new work for the queue
    @param[in] priority specify the priority of the work, can be low,
    medium(default), or high.  High is always done first, low and medium
    alternate with medium jobs executing more often
    */
        void addWorkBlock(
            std::shared_ptr<BasicWorkBlock> newWork,
            WorkPriority priority = WorkPriority::medium)
        {
            if ((!newWork) ||
                (newWork->isFinished() && priority != WorkPriority::required)) {
                return;
            }
            if (numWorkers > 0) {
                size_t ccount;
                switch (priority) {
                    case WorkPriority::high:
                    case WorkPriority::required:
                        ccount = workToDoHigh.size();
                        workToDoHigh.push(std::move(newWork));
                        break;
                    case WorkPriority::medium:
                        ccount = workToDoMed.size();
                        workToDoMed.push(std::move(newWork));
                        break;
                    case WorkPriority::low:
                    default:
                        ccount = workToDoLow.size();
                        workToDoLow.push(std::move(newWork));
                        break;
                }
                if (static_cast<int>(ccount) <= numWorkers) {
                    queueCondition.notify_one();
                }
            } else {
                newWork->execute();
            }
        }
        /** add a block of work to the WorkQueue
    @param[in] newWork  a vector of workBlocks to add to the queue
    */
        void addWorkBlock(
            std::vector<std::shared_ptr<BasicWorkBlock>>& newWork,
            WorkPriority priority = WorkPriority::medium)
        {
            if (numWorkers > 0) {
                switch (priority) {
                    case WorkPriority::high:
                    case WorkPriority::required:
                        workToDoHigh.pushVector(newWork);
                        break;
                    case WorkPriority::medium:
                        workToDoMed.pushVector(newWork);
                        break;
                    case WorkPriority::low:
                    default:
                        workToDoLow.pushVector(newWork);
                        break;
                }
                queueCondition.notify_all();
            } else {
                for (auto& wb : newWork) {
                    wb->execute();
                }
            }
        }
        /** check if the queue is empty
    @details this function may not be that useful since it is multithreaded and
    the answer is not necessarily valid after it returns
    @return true if the queue is empty false if it has work to do
    */
        bool isEmpty() const
        {
            return (
                workToDoHigh.empty() && workToDoMed.empty() &&
                workToDoLow.empty());
        };
        /** get the number of remaining blocks
     @details this function may not be that useful since it is multithreaded and
    the answer is not necessarily valid after it returns
    */
        size_t numBlock() const
        {
            return (
                workToDoHigh.size() + workToDoMed.size() + workToDoLow.size());
        };
        /** set the ratio of medium block executions to low priority block
    executions
    @param[in] newPriorityRatio  if >0 used as the new ratio otherwise set to
    default value*/
        void setPriorityRatio(int newPriorityRatio)
        {
            priorityRatio = (newPriorityRatio > 0) ? newPriorityRatio :
                                                     defaultPriorityRatio;
        };
        /** get the next work block
    @return a shared pointer to a work block
    */
        std::shared_ptr<BasicWorkBlock> getWorkBlock()
        {
            std::shared_ptr<BasicWorkBlock> wb;
            auto wbb = workToDoHigh.pop();
            if (wbb) {
                return *wbb;
            }

            if (MedCounter >= priorityRatio) {
                if (!workToDoLow.empty()) {
                    wbb = workToDoLow.pop();
                    if (wbb) {
                        MedCounter = 0;
                        return *wbb;
                    }
                }
            }
            wbb = workToDoMed.pop();
            if (wbb) {
                ++MedCounter;
                return *wbb;
            }
            wbb = workToDoLow.pop();
            if (wbb) {
                return *wbb;
            }

            return wb;
        }

      private:
        /** the main worker loop*/
        void workerLoop()
        {
            while (true) {
                if (isEmpty()) {
                    std::unique_lock<std::mutex> lv(queueLock);
                    if (halt.load()) {
                        return;
                    }
                    queueCondition.wait_for(
                        lv, std::chrono::milliseconds(5000));
                    if (halt) {
                        return;
                    }
                }
                auto wb =
                    getWorkBlock();  // this will return empty if it is spurious
                                     // and also sync the size if needed
                if ((wb) && (!wb->isFinished())) {
                    wb->execute();
                }
            }
        }

      private:
        WorkQueue(WorkQueue const&) = delete;
        WorkQueue& operator=(WorkQueue const&) = delete;

        std::atomic<int> priorityRatio{defaultPriorityRatio};  //!< the ratio of
                                                               //!< medium
                                                               //!< Priority
                                                               //!< blocks to
                                                               //!< low
        //!< priority blocks

        SimpleQueue<std::shared_ptr<BasicWorkBlock>>
            workToDoHigh;  //!< queue containing the work to do
        SimpleQueue<std::shared_ptr<BasicWorkBlock>>
            workToDoMed;  //!< queue containing the work to do
        SimpleQueue<std::shared_ptr<BasicWorkBlock>>
            workToDoLow;  //!< queue containing the work to do
        const int numWorkers;  //!< counter for the number of workers
        std::atomic<int> MedCounter{
            0};  //!< the counter to use low instead of Med
        std::vector<std::thread> threadpool;  //!< the threads
        std::mutex queueLock;  //!< mutex for condition variable and halt
        std::condition_variable queueCondition;  //!< condition variable for
                                                 //!< waking the threads
        std::atomic<bool> halt{
            false};  //!< flag indicating the threads should halt
    };

}  // namespace containers
}  // namespace gmlc
