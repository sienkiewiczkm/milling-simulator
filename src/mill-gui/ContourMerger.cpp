#include "ContourMerger.hpp"
#include "PointQuadtree.hpp"
#include "GeometricIntersections.hpp"

namespace ms
{


ContourMerger::ContourMerger()
{
}

ContourMerger::~ContourMerger()
{
}

std::vector<glm::dvec3> ContourMerger::merge2D(
    const std::vector<glm::dvec3> &lhs,
    const std::vector<glm::dvec3> &rhs
) const
{
    return lhs;
}

}
