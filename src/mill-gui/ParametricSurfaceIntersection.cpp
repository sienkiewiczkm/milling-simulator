#include "ParametricSurfaceIntersection.hpp"

namespace fw
{

ParametricSurfaceIntersection::ParametricSurfaceIntersection():
    lhsParameters{},
    rhsParameters{},
    scenePosition{}
{
}

ParametricSurfaceIntersection::ParametricSurfaceIntersection(
    glm::dvec2 lhsParameters,
    glm::dvec2 rhsParameters,
    glm::dvec3 scenePosition
):
    lhsParameters{lhsParameters},
    rhsParameters{rhsParameters},
    scenePosition{scenePosition}
{
}

}
