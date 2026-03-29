// main.cpp
#include "gtest/gtest.h"

int main(int argc, char** argv) {
    setlocale(LC_ALL, "rus");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
