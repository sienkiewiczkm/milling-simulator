#pragma once

#include "BsplineSurface.hpp"
#include <memory>

namespace fw
{

std::shared_ptr<BsplineSurface> createBsplinePlane(
    glm::dvec3 uv0,
    glm::dvec3 u1,
    glm::dvec3 v1,
    glm::dvec3 uv1,
    glm::dmat4 controlPointTransformation = glm::dmat4(),
    glm::ivec2 controlPointsGridSize = {16, 16}
);

}
