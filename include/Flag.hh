#pragma once

#include <dispatch/dispatch.h>


class Flag {

public:
    Flag (
            long initialCount = 0
            );

    Flag (
            Flag&& flag
            );

    Flag (
            const Flag& flag
            ) = delete;

    virtual ~Flag ();

    void post ();

    bool wait ();

    void cancel ();

private:
    dispatch_semaphore_t sem_;
    bool cancelled_;
};
