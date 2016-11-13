#include "PointQuadtree.hpp"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "glm/glm.hpp"

#include <memory>

using ::testing::UnorderedElementsAre;

class PointQuadtreeTests:
    public ::testing::Test
{
public:
    virtual void SetUp() override
    {
        _quadtree = std::make_shared<fw::PointQuadtree<glm::vec2, int>>(
            fw::AABB<glm::vec2>{{0, 0}, {1.0f, 1.0f}}
        );
    }

    virtual void TearDown() override
    {
    }

    std::shared_ptr<fw::PointQuadtree<glm::vec2, int>> _quadtree;
};

TEST_F(PointQuadtreeTests, ShouldInitializeEmpty)
{
    EXPECT_EQ(0, _quadtree->getNumElements());
}

TEST_F(PointQuadtreeTests, ShouldBeNotEmptyAfterAddingInRegion)
{
    _quadtree->addElement(glm::vec2{0, 0.5f}, std::make_shared<int>(2));
    EXPECT_EQ(1, _quadtree->getNumElements());
}

TEST_F(PointQuadtreeTests, ShouldBeStillEmptyAfterAddingOutsideRegion)
{
    _quadtree->addElement(glm::vec2{1.1f, 0.5f}, std::make_shared<int>(2));
    EXPECT_EQ(0, _quadtree->getNumElements());
}

TEST_F(PointQuadtreeTests, ShouldReturnElementsInArea)
{
    auto p1 = std::make_shared<int>(1);
    auto p2 = std::make_shared<int>(2);
    auto p3 = std::make_shared<int>(3);
    auto p4 = std::make_shared<int>(4);
    auto p5 = std::make_shared<int>(5);

    _quadtree->addElement(glm::vec2{0.25f, 0.25f}, p1);
    _quadtree->addElement(glm::vec2{0.66f, 0.66f}, p2);
    _quadtree->addElement(glm::vec2{0.83f, 0.66f}, p3);
    _quadtree->addElement(glm::vec2{0.66f, 0.83f}, p4);
    _quadtree->addElement(glm::vec2{0.83f, 0.83f}, p5);

    fw::AABB<glm::vec2> queryRegion{ {0.65f, 0.65f}, {0.85f, 0.85f} };
    auto foundElements = _quadtree->findElements(queryRegion);
    EXPECT_EQ(4, foundElements.size());
    EXPECT_THAT(foundElements, UnorderedElementsAre(p2, p3, p4, p5));
}

