#include "Flag.hh"
#include <stdexcept>

Flag::Flag (
        long initialCount
        )
: sem_(dispatch_semaphore_create(initialCount))
, cancelled_(false)
{
    if (!sem_) {
        throw std::runtime_error("Unable to create Flag semaphore");
    }
}


Flag::Flag (
        Flag&& flag
        )
{
    sem_ = flag.sem_;
    cancelled_ = flag.cancelled_;

    flag.sem_ = nullptr;
}


Flag::~Flag ()
{
    if (sem_) {
        dispatch_release(sem_);
    }
}


void Flag::post ()
{
    if (cancelled_) {
        return;
    }

    dispatch_semaphore_signal(sem_);
}


bool Flag::wait ()
{
    dispatch_semaphore_wait(sem_, DISPATCH_TIME_FOREVER);

    return !cancelled_;
}


void Flag::cancel ()
{
    cancelled_ = true;
    dispatch_semaphore_signal(sem_);
}
