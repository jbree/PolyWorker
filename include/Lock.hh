#pragma once

#include <unistd.h>
#include <pthread.h>

class Lock {

public:
    Lock ();
    Lock (const Lock&) = delete;

    virtual ~Lock ();

    void lock ();

    void unlock ();

private:
#ifdef _POSIX_THREADS
    pthread_mutex_t mutex_;
#else
#error "Lock class depends on pthreads"
#endif
};

