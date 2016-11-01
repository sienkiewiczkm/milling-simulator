#include "BsplineEquidistantKnotGenerator.hpp"
#include <gtest/gtest.h>
#include <memory>

class BsplineEquidistantKnotGeneratorTests:
    public ::testing::Test
{
public:
    virtual void SetUp() override
    {
        _generator = std::make_shared<fw::BsplineEquidistantKnotGenerator>();
        _knots = _generator->generate(15, 4);
    }

    virtual void TearDown() override
    {
    }

    std::shared_ptr<fw::BsplineEquidistantKnotGenerator> _generator;
    std::vector<double> _knots;
};

TEST_F(BsplineEquidistantKnotGeneratorTests, ShouldReturnCorrectAmountOfKnots)
{
    EXPECT_EQ(20, _knots.size());
}

TEST_F(BsplineEquidistantKnotGeneratorTests, ShouldHaveZeroOnFirstPlace)
{
    EXPECT_DOUBLE_EQ(0.0, _knots[0]);
}

TEST_F(BsplineEquidistantKnotGeneratorTests, ShouldHaveOneAtLastPosition)
{
    EXPECT_DOUBLE_EQ(1.0, _knots.back());
}

TEST_F(BsplineEquidistantKnotGeneratorTests, ShouldReturnEquidistantValues)
{
    auto firstDifference = _knots[1] - _knots[0];
    for (auto i = 2; i < _knots.size(); ++i)
    {
        EXPECT_NEAR(firstDifference, _knots[i] - _knots[i-1], 0.000001);
    }
}
