#include "PolyWorker.hh"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>


/// A callback typedef used to perform work
int doWork (int&& input, void* pUser)
{
    std::ostringstream ss;
    ss << "do work: " << input << std::endl;
    std::cout << ss.str();
    return input * 2;
}

/// A callback typedef used when a piece of work is completed
void workComplete(int&& output, void* pUser)
{
    std::ostringstream ss;
    ss << "work done: " << output << std::endl;
    std::cout << ss.str();

}

/// A callback typedef used when all work is completed
void allWorkComplete(void* pUser)
{
    std::cout << "all work completed\n";
}

TEST(PolyWorkerTest, start)
{
    PolyWorker<int, int> pw(4, doWork, workComplete, allWorkComplete);
    pw.start();

    for (int i(0); i < 100; i++) {
        int j(i);
        pw.addWork(std::move(j));
    }
}