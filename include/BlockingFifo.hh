#pragma once

#include "Flag.hh"
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>

template <typename T>
class BlockingFifo {

public:

    BlockingFifo ()
    {}

    ~BlockingFifo ()
    {}


    void push (std::shared_ptr<T> t)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        items_.push(t);

        flag_.post();
    }

    std::shared_ptr<T> pop ()
    {
        flag_.wait();

        std::lock_guard<std::mutex> lock(mutex_);
        
        auto ptr = items_.front();
        items_.pop();

        return ptr;
    }

    friend std::ostream& operator<< (
            std::ostream& out,
            const BlockingFifo& fifo
            )
    {
        return out << fifo.items_.size();
    }


private:

    Flag flag_;
    std::mutex mutex_;
    std::queue<std::shared_ptr<T>> items_;
};