#pragma once
#include <glm/glm.hpp>

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

}
