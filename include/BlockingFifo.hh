#include "Flag.hh"
#include "Lock.hh"
#include <iostream>
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
        mutex_.lock();

        items_.push(t);

        mutex_.unlock();

        flag_.post();
    }

    std::shared_ptr<T> pop ()
    {
        flag_.wait();

        mutex_.lock();
        
        auto ptr = items_.front();
        items_.pop();

        mutex_.unlock();

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

    Lock mutex_;
    Flag flag_;
    std::queue<std::shared_ptr<T>> items_;
};