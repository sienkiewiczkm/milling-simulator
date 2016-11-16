#include "GeometricIntersections.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "glm/glm.hpp"

TEST(intersectSegments, ShouldHandleCollinearCaseSameDirection)
{
    auto result = fw::intersectSegments<glm::dvec2, double>(
        {1.0, 1.0},
        {2.0, 2.0},
        {-1.0, -1.0},
        {10.0, 10.0}
    );

    EXPECT_EQ(fw::GeometricIntersectionKind::Infinite, result.kind);
}

TEST(intersectSegments, ShouldHandleCollinearNonColliding)
{
    auto result = fw::intersectSegments<glm::dvec2, double>(
        {1.0, 1.0},
        {2.0, 2.0},
        {5.0, 5.0},
        {10.0, 10.0}
    );

    EXPECT_EQ(fw::GeometricIntersectionKind::None, result.kind);
}

TEST(intersectSegments, ShouldHandleParallelCase)
{
    auto result = fw::intersectSegments<glm::dvec2, double>(
        {1.0, 1.0},
        {3.0, 3.0},
        {1.0, 4.0},
        {5.0, 8.0}
    );

    EXPECT_EQ(fw::GeometricIntersectionKind::None, result.kind);
}

TEST(intersectSegments, ShouldHandleSingleIntersection)
{
    auto result = fw::intersectSegments<glm::dvec2, double>(
        {1.0, 1.0},
        {2.0, 2.0},
        {1.0, 2.0},
        {2.0, 1.0}
    );

    EXPECT_EQ(fw::GeometricIntersectionKind::Single, result.kind);
    EXPECT_DOUBLE_EQ(0.5, result.t0);
}

TEST(intersectSegments, ShouldHandlePerpendicularNonColliding)
{
    auto result = fw::intersectSegments<glm::dvec2, double>(
        {1.0, 1.0},
        {3.0, 3.0},
        {3.0, 5.0},
        {5.0, 3.0}
    );

    EXPECT_EQ(fw::GeometricIntersectionKind::None, result.kind);
}

TEST(intersectSegments, ShouldHandleNonParallelCommonEnd)
{
    auto result = fw::intersectSegments<glm::dvec2, double>(
        {1.0, 1.0},
        {3.0, 3.0},
        {3.0, 3.0},
        {5.0, 1.0}
    );

    EXPECT_EQ(fw::GeometricIntersectionKind::Single, result.kind);
    EXPECT_DOUBLE_EQ(1.0, result.t0);
}

