#include <gtest/gtest.h>
#include "../include/example.h"

bool f() {
    return true;
}

TEST(ExampleTest, DemostrateGTestMacros) {
    EXPECT_EQ(true, true);
    const bool result = f();
    EXPECT_EQ(true, result) << "Hello, World!";
}

TEST(ExampleTest, Square) {
    int x = 5;
    int expectedSquare = x * x;
    EXPECT_EQ(
        expectedSquare,
        square(x)
    );
}