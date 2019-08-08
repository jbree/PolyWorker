#include "Flag.hh"
#include <array>
#include <gtest/gtest.h>
#include <thread>

void flag_post_true(Flag& flag)
{
    EXPECT_TRUE(flag.wait());
}

TEST(FlagTest, OneWaitPerPost)
{
    Flag flag { 5 };

    flag.cancel();

    for (size_t i(0); i < 5; i++) {
        EXPECT_TRUE(flag.wait());
    }

    EXPECT_FALSE(flag.wait());
}


TEST(FlagTest, PostThenWait)
{
    Flag flag;

    flag.post();

    EXPECT_TRUE(flag.wait());

    flag.post();

    EXPECT_TRUE(flag.wait());

    flag.cancel();

    EXPECT_FALSE(flag.wait());
}


TEST(FlagTest, CancelCancelsAllWhenNoMorePosts)
{
    Flag flag { 0 };

    std::vector<std::shared_ptr<std::thread>> threads;

    for (size_t i(0); i < 5; i++) {
        threads.push_back(std::make_shared<std::thread>([&](){ EXPECT_FALSE(flag.wait()); }));
    }

    flag.cancel();

    for (auto pThread: threads) {
        pThread->join();
    }
}


TEST(FlagTest, CancelFinishesPostedWork)
{
    Flag flag { 5 };

    std::vector<std::shared_ptr<std::thread>> threads;

    flag.cancel();

    for (size_t i(0); i < 5; i++) {
        threads.push_back(std::make_shared<std::thread>([&](){ EXPECT_TRUE(flag.wait()); }));
    }

    for (auto pThread: threads) {
        pThread->join();
    }
}


TEST(FlagTest, AbortIgnoresPostedWork)
{
    Flag flag { 5 };

    std::vector<std::shared_ptr<std::thread>> threads;

    flag.abort();

    for (size_t i(0); i < 5; i++) {
        threads.push_back(std::make_shared<std::thread>([&](){ EXPECT_FALSE(flag.wait()); }));
    }

    for (auto pThread: threads) {
        pThread->join();
    }
}



TEST(FlagTest, AbortCancelsAll)
{
    Flag flag { 0 };

    std::vector<std::shared_ptr<std::thread>> threads;

    for (size_t i(0); i < 5; i++) {
        threads.push_back(std::make_shared<std::thread>([&](){ EXPECT_FALSE(flag.wait()); }));
    }

    flag.abort();

    for (auto pThread: threads) {
        pThread->join();
    }
}