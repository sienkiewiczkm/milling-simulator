#include "LinearCombinationEvaluator.hpp"
#include "IBasisEvaluatorMock.hpp"

#include <glm/glm.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

using ::testing::Return;
using TestEvaluator = fw::LinearCombinationEvaluator<glm::dvec3>;

class LinearCombinationEvaluatorTests:
    public ::testing::Test
{
public:
    virtual void SetUp() override
    {
        std::vector<glm::dvec3> controlPoints {
            glm::dvec3(1.0, 0.0, 0.0),
            glm::dvec3(2.0, 1.0, 0.0),
            glm::dvec3(5.0, 2.0, 1.0),
            glm::dvec3(7.0, 5.0, 2.0),
        };

        _basisEvaluatorMock = std::make_shared<IBasisEvaluatorMock>();
        _evaluator = std::make_shared<TestEvaluator >(
            std::static_pointer_cast<fw::IBasisEvaluator>(_basisEvaluatorMock),
            controlPoints
        );
    }

    std::vector<double> _knots;
    std::shared_ptr<IBasisEvaluatorMock> _basisEvaluatorMock;
    std::shared_ptr<TestEvaluator> _evaluator;
};

TEST_F(LinearCombinationEvaluatorTests, ShouldReturnZeroWhenZeroBasis)
{
    std::vector<double> basis { 0.0, 0.0, 0.0, 0.0 };
    EXPECT_CALL(*_basisEvaluatorMock, evaluate(1.0))
        .WillOnce(Return(basis));

    auto evaluated = _evaluator->evaluate(1.0);
    EXPECT_DOUBLE_EQ(0.0, evaluated.x);
    EXPECT_DOUBLE_EQ(0.0, evaluated.y);
    EXPECT_DOUBLE_EQ(0.0, evaluated.z);
}

TEST_F(LinearCombinationEvaluatorTests, ShouldReturnCorrectCombination)
{
    std::vector<double> basis { 1.0, 2.0, 1.0, -1.0 };
    EXPECT_CALL(*_basisEvaluatorMock, evaluate(1.0))
        .WillOnce(Return(basis));

    auto evaluated = _evaluator->evaluate(1.0);
    EXPECT_DOUBLE_EQ(3.0, evaluated.x);
    EXPECT_DOUBLE_EQ(-1.0, evaluated.y);
    EXPECT_DOUBLE_EQ(-1.0, evaluated.z);
}
