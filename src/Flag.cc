#include "Flag.hh"
#include <stdexcept>

Flag::Flag (
        long initialCount
        )
: cancelled_(false)
, count_(0)
{
}


Flag::~Flag ()
{
}


void Flag::post ()
{
    if (cancelled_) {
        return;
    }

    std::lock_guard<decltype(mutex_)> lock(mutex_);
    ++count_;
    condition_.notify_one();
}


bool Flag::wait ()
{
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    while(!count_) {
        condition_.wait(lock);
    }

    --count_;
    return !cancelled_;
}


void Flag::cancel ()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    cancelled_ = true;
    count_ = 0;
    condition_.notify_all();
}
