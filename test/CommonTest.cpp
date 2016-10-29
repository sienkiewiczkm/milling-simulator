#include <gtest/gtest.h>
#include "Common.hpp"

TEST(CommonTests, PiShouldBeAroundTrueValue)
{
    auto piValue = common::pi();
    EXPECT_TRUE(3.13 < piValue && piValue < 3.15);
}

