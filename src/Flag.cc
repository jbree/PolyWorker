#include "Flag.hh"
#include <stdexcept>

Flag::Flag (
        long initialCount
        )
: cancelled_(false)
, cancelling_(false)
, count_(initialCount)
{
}


Flag::~Flag ()
{
}


void Flag::post ()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (cancelled_ || cancelling_) {
        return;
    }

    ++count_;
    condition_.notify_one();
}


bool Flag::wait ()
{
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    while (!count_ && !cancelling_) {
        condition_.wait(lock);
    }

    // Ordering is imporant here. Only decrement when we aren't cancelled. Only
    // set to cancelled if count goes down to zero and we're currently
    // cancelling. And if we decrement to zero while cancelling, this is a
    // special case where true gets returned one last time.
    if (!cancelled_ && --count_ == 0 && cancelling_) {
        cancelled_ = true;
        return true;
    }

    return !cancelled_;
}


void Flag::cancel ()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    cancelling_ = true;

    // If count is already at zero, insta-cancel.
    cancelled_ = (count_ == 0);

    condition_.notify_all();
}


void Flag::abort ()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);

    cancelling_ = true;
    cancelled_ = true;
    count_ = 0;

    condition_.notify_all();
}
