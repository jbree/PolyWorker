#pragma once

#include "Flag.hh"
#include "WorkerThread.hh"
#include <iostream>
#include <queue>
#include <thread>


template <typename InputT, typename OutputS>
class PolyWorker {

/// A callback typedef used to perform work
typedef OutputS (*DoWork)(InputT&&, void* pUser);

/// A callback typedef used when a piece of work is completed
typedef void (*WorkComplete)(OutputS&&, void* pUser);

/// A callback typedef used when all work is completed
typedef void (*AllWorkComplete)(void* pUser);

public:
    PolyWorker (
        size_t workerCount,
        DoWork doWorkCallback,
        WorkComplete workCompleteCallback,
        AllWorkComplete allWorkCompleteCallback
    )
    : doWork_(doWorkCallback)
    , workComplete_(workCompleteCallback)
    , allWorkComplete_(allWorkCompleteCallback)
    {
    }


    ~PolyWorker ()
    {
        if (synchronizeThread_) {
            synchronizeThread_->join();
        } 
    }


    void start ()
    {
        synchronizeThread_.reset(new std::thread(&PolyWorker::synchronizer, this));

        workers_.emplace(workers_.end(), &PolyWorker::worker, this);
    }

    void addWork (
            InputT&& todo
            )
    {
        std::cout << "addWork\n";
        Work<InputT, OutputS> work;
        work.input = todo;

        availableMutex_.lock();
        workAvailable_.push(std::move(work));
        availableMutex_.unlock();

        work_.post();
    }


    Work<InputT, OutputS>& getWork ()
    {
        std::cout << "getWork\n";
        work_.wait();
    
        availableMutex_.lock();
        inProgressMutex_.lock();

        workInProgress_.push(std::move(workAvailable_.front()));
        auto& work(workInProgress_.back());
        workAvailable_.pop();

        inProgressMutex_.unlock();
        availableMutex_.unlock();

        return work;
    }

private:

    void synchronizer ()
    {
        std::cout << "synchronizer invoked" << std::endl;

        // wait for work to be added to in-progress queue
        while (inProgressFlag_.wait()) {
            std::cout << "found work in progress" << std::endl;
            inProgressMutex_.lock();

            Work<InputT, OutputS> work(std::move(workInProgress_.front()));
            workInProgress_.pop();

            inProgressMutex_.unlock();

            // wait for in-progress work to be completed, then invoke callback
            work.flag.wait();
            workComplete_(std::move(work.output), 0);

            std::cout << "posted work in progress" << std::endl;
        }
    }


    void worker ()
    {

        while (true) {
            std::cout << "worker\n";
            auto& work(getWork());

            work.output = doWork_(std::move(work.input), 0);

            work.flag.post();
        }

    }

    std::unique_ptr<std::thread> synchronizeThread_;


    std::queue<Work<InputT, OutputS>> workInProgress_;
    std::queue<Work<InputT, OutputS>> workAvailable_;

    /// indicates available work
    Flag work_;

    std::vector<std::thread> workers_;

    /// protects workInProgress_ queue
    Lock inProgressMutex_;
    Flag inProgressFlag_;

    /// protects workAvailable_ queue
    Lock availableMutex_;

    DoWork doWork_;
    WorkComplete workComplete_;
    AllWorkComplete allWorkComplete_;

};
