#pragma once

#include "IParametricSurfaceUV.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace fw
{

struct ParametricSurfaceIntersection
{
public:
    ParametricSurfaceIntersection();
    ParametricSurfaceIntersection(
        glm::dvec2 lhsParameters,
        glm::dvec2 rhsParameters,
        glm::dvec3 scenePosition
    );

    glm::dvec2 lhsParameters;
    glm::dvec2 rhsParameters;
    glm::dvec3 scenePosition;
};

class ParametricSurfaceIntersectionFinder
{
public:
    ParametricSurfaceIntersectionFinder();
    virtual ~ParametricSurfaceIntersectionFinder();

    std::vector<ParametricSurfaceIntersection> intersect(
        const std::shared_ptr<IParametricSurfaceUV> lhs,
        const std::shared_ptr<IParametricSurfaceUV> rhs,
        glm::vec3 neighbourhoodPoint
    );
};

}
