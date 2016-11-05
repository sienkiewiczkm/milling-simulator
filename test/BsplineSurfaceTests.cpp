#include "BsplineSurface.hpp"
#include "IBsplineKnotGeneratorMock.hpp"

using ::testing::Return;

class BsplineSurfaceTests:
    public ::testing::Test
{
public:
    virtual void SetUp() override
    {
        _controlPoints = {
            {1.0, 0.0, 1.0}, {2.0, 0.0, 1.0}, {3.0, 0.0, 1.0}, {5.0, 0.0, 1.0},
            {1.0, 0.0, 2.0}, {2.0, 0.0, 2.0}, {3.0, 0.0, 2.0}, {4.0, 0.0, 2.0},
            {1.0, 0.0, 3.0}, {2.0, 0.0, 3.0}, {3.0, 0.0, 3.0}, {4.0, 0.0, 3.0},
            {1.0, 0.0, 4.0}, {2.0, 0.0, 4.0}, {3.0, 0.0, 4.0}, {4.0, 0.0, 4.0},
        };

        _knots = { 0.0, 1.0/7, 2.0/7, 3.0/7, 4.0/7, 5.0/7, 6.0/7, 1.0 };

        _surface = std::make_shared<fw::BsplineSurface>(
            3,
            glm::ivec2(4, 4),
            _controlPoints,
            _knots,
            _knots
        );
    }

    virtual void TearDown() override
    {
    }

protected:
    std::vector<double> _knots;
    std::vector<glm::dvec3> _controlPoints;
    std::shared_ptr<fw::BsplineSurface> _surface;
};

TEST_F(BsplineSurfaceTests, ShouldReturnCornerPositionForZeroParameters)
{
    auto zeroPoint = 3.0/7;
    auto point = _surface->getPosition(glm::dvec2(zeroPoint, zeroPoint));
    EXPECT_DOUBLE_EQ(2.0, point.x);
    EXPECT_DOUBLE_EQ(0.0, point.y);
    EXPECT_DOUBLE_EQ(2.0, point.z);
}

TEST_F(BsplineSurfaceTests, ShouldReturnLastCornerPositionForMaxParameters)
{
    auto onePoint = 4.0/7;
    auto point = _surface->getPosition(glm::dvec2(onePoint, onePoint));
    EXPECT_DOUBLE_EQ(3.0, point.x);
    EXPECT_DOUBLE_EQ(0.0, point.y);
    EXPECT_DOUBLE_EQ(3.0, point.z);
}
