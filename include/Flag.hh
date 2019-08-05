#pragma once

#include <condition_variable>
#include <mutex>


class Flag {

public:
    Flag (
            long initialCount = 0
            );

    Flag (
            const Flag& flag
            ) = delete;

    virtual ~Flag ();

    void post ();

    bool wait ();

    void cancel ();

private:
    std::mutex mutex_;
    std::condition_variable condition_;
    
    bool cancelled_;
    uint32_t count_;
};
