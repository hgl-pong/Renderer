#include <gtest/gtest.h>
//#include "TestJobSystem.h"
//#include "TestWindow.h"
//#include "TestJsonParser.h"
#include "TestParallel.h"

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
