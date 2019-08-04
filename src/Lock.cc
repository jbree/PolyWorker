#include "Lock.hh"

Lock::Lock ()
{
    pthread_mutex_init(&mutex_, 0);
}


Lock::~Lock ()
{
    pthread_mutex_destroy(&mutex_);
}


void Lock::lock ()
{
    while (pthread_mutex_lock(&mutex_) != 0);
}


void Lock::unlock ()
{
    while (pthread_mutex_unlock(&mutex_) != 0);
}
