#include "PolyWorker.hh"
#include <gtest/gtest.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <sstream>


/// A callback typedef used to perform work
double doWork (int&& input, void* pUser)
{
    return std::sqrt(input);
}

/// A callback typedef used when a piece of work is completed
void workComplete(double&& output, void* pUser)
{
    std::ofstream& fs(*static_cast<std::ofstream*>(pUser));

    fs << output << std::endl;
}

/// A callback typedef used when all work is completed
void allWorkComplete(void* pUser)
{
    std::cout << "all work completed\n";
}

TEST(PolyWorkerTest, start)
{
    std::ofstream fs("poly.txt");
    PolyWorker<int, double> pw(8, doWork, workComplete, allWorkComplete, &fs);

    for (int i(0); i < 10000000; i++) {
        int j(i);

        pw.addWork(std::move(j));
    }
    pw.endWork();
}