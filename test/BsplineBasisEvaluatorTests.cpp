#include "BsplineBasisEvaluator.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

using testing::ElementsAre;

class BsplineBasisEvaluatorTests:
    public ::testing::Test
{
public:
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }

    void prepareDegree(int degree)
    {
        std::vector<double> knots {0.0, 1.0, 2.0, 3.0};
        _evaluator = std::make_shared<fw::BsplineBasisEvaluator>(
            degree,
            knots
        );
    }

    std::shared_ptr<fw::BsplineBasisEvaluator> _evaluator;
};

TEST_F(BsplineBasisEvaluatorTests, ShouldEvaluateCorrectZeroDegreeBasis)
{
    prepareDegree(0);

    {
        auto basis = _evaluator->evaluate(0.0);
        EXPECT_THAT(basis, ElementsAre(1.0, 0.0, 0.0));
    }
    {
        auto basis = _evaluator->evaluate(1.0);
        EXPECT_THAT(basis, ElementsAre(0.0, 1.0, 0.0));
    }
    {
        auto basis = _evaluator->evaluate(2.0);
        EXPECT_THAT(basis, ElementsAre(0.0, 0.0, 1.0));
    }
}

TEST_F(BsplineBasisEvaluatorTests, ShouldEvaluateCorrectFirstDegreeBasis)
{
    const int degree = 1;
    prepareDegree(degree);

    {
        auto basis = _evaluator->evaluate(1.0);
        EXPECT_THAT(basis, ElementsAre(1.0, 0.0));
    }
    {
        auto basis = _evaluator->evaluate(1.5);
        EXPECT_THAT(basis, ElementsAre(0.5, 0.5));
    }
    {
        auto basis = _evaluator->evaluate(2.0);
        EXPECT_THAT(basis, ElementsAre(0.0, 1.0));
    }
}

TEST_F(BsplineBasisEvaluatorTests, ShouldEvaluateCorrectSecondDegreeBasis)
{
    const int degree = 2;
    prepareDegree(degree);

    {
        auto basis = _evaluator->evaluate(0.0);
        EXPECT_THAT(basis, ElementsAre(0.0));
    }
    {
        auto basis = _evaluator->evaluate(0.5);
        EXPECT_THAT(basis, ElementsAre(0.125));
    }
    {
        auto basis = _evaluator->evaluate(1.0);
        EXPECT_THAT(basis, ElementsAre(0.5));
    }
    {
        auto basis = _evaluator->evaluate(1.5);
        EXPECT_THAT(basis, ElementsAre(0.75));
    }
    {
        auto basis = _evaluator->evaluate(2.0);
        EXPECT_THAT(basis, ElementsAre(0.5));
    }
    {
        auto basis = _evaluator->evaluate(2.5);
        EXPECT_THAT(basis, ElementsAre(0.125));
    }
    {
        auto basis = _evaluator->evaluate(3.0);
        EXPECT_THAT(basis, ElementsAre(0.0));
    }
}
