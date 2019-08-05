#pragma once

#include "BlockingFifo.hh"
#include "Flag.hh"
#include "WorkerThread.hh"
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
        AllWorkComplete allWorkCompleteCallback,
        void* pUser
    )
    : doWork_(doWorkCallback)
    , workComplete_(workCompleteCallback)
    , allWorkComplete_(allWorkCompleteCallback)
    , pUser_(pUser)
    , finished_(false)
    , synchronizer_(&PolyWorker::synchronizer, this)
    {
        for (size_t i(0); i < workerCount; i++) {
            workers_.emplace(workers_.end(), &PolyWorker::worker, this);
        }
    }


    ~PolyWorker ()
    {
        for (auto& thr: workers_) {
            thr.join();
        }

        synchronizer_.join();
    }


    void addWork (
            InputT&& todo
            )
    {
        auto work = std::make_shared<Work<InputT, OutputS>>();
        work->input = std::move(todo);
        workAvailable_.push(work);
    }


    void endWork ()
    {
        for (size_t i(0); i < workers_.size(); i++) {
            workAvailable_.push(nullptr);
        }
    }

private:

    std::shared_ptr<Work<InputT, OutputS>> getWork ()
    {
        std::lock_guard<std::mutex> lock(workTransfer_);

        auto work(workAvailable_.pop());
        
        if (!work) {
            if (!finished_) {
                finished_ = true;
                workInProgress_.push(nullptr);
            }
            
            workTransfer_.unlock();
            return nullptr;
        }

        workInProgress_.push(work);

        return work;
    }


    void synchronizer ()
    {
        // Wait for work to be added to in-progress queue
        std::shared_ptr<Work<InputT, OutputS>> work;
        
        while ((work = workInProgress_.pop())) {
            // Wait for in-progress work to be completed, then invoke callback
            work->flag.wait();

            workComplete_(std::move(work->output), pUser_);
        }

        allWorkComplete_(pUser_);
    }


    void worker ()
    {
        std::shared_ptr<Work<InputT, OutputS>> work;
        while ((work = getWork())) {
            work->output = doWork_(std::move(work->input), pUser_);

            work->flag.post();
        }
    }

    DoWork doWork_;
    WorkComplete workComplete_;
    AllWorkComplete allWorkComplete_;
    void* pUser_;

    /// Contains all added work which has not yet been started
    BlockingFifo<Work<InputT, OutputS>> workAvailable_;

    /// Contains all work in progress and completed work which has not yet been
    /// posted.
    BlockingFifo<Work<InputT, OutputS>> workInProgress_;

    /// Prevents multiple 
    bool finished_;

    /// protects workInProgress_ queue
    std::mutex workTransfer_;

    std::thread synchronizer_;
    std::vector<std::thread> workers_;
};
