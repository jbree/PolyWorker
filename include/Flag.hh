#pragma once

#include <condition_variable>
#include <mutex>


class Flag {

public:

    /// Construct a flag with specified initial count
    Flag (
            long initialCount = 0
            );

    Flag (
            const Flag& flag
            ) = delete;

    virtual ~Flag ();

    /// Post to flag, incrementing its counter by one. If the flag has been 
    /// cancelled or aborted, this method does nothing.
    void post ();


    /// Wait for the flag. During normal operation, `wait()` returns true once
    /// for each time the Flag was `post()`ed, blocking until post is called if
    /// necessary.
    ///
    /// When the Flag is cancelled, it will continue to return true for each 
    /// previously made post. Any subsequent invocations will return 0,
    /// indicating cancellation.
    ///
    /// When the Flag is aborted, wait will immediately return false for every
    /// thread currently blocking on this call, as well as every future
    /// invocation.
    bool wait ();


    /// Cancel this flag, causing future `post()`s to do nothing. See `wait()`
    /// for more information.
    void cancel ();


    /// Abort this Flag, causing currently blocking and future `wait()`
    /// invocations to immediately return false.
    void abort ();

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    
    bool cancelled_;
    bool cancelling_;
    uint32_t count_;
};
