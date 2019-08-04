#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <cmath>


TEST(MonoWorkerTest, start)
{
    std::ofstream file("mono.txt");

    for (int i(0); i < 100000; i++) {
        file << std::sqrt(i) << std::endl;
    }

}