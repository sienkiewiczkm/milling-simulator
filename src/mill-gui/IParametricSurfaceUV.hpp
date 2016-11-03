#pragma once
#include <glm/glm.hpp>

namespace fw
{

class IParametricSurfaceUV
{
public:
    virtual ~IParametricSurfaceUV() = default;

    virtual glm::dvec3 getPosition(glm::dvec2 parametrisation) = 0;
    virtual glm::dvec3 getNormal(glm::dvec2 parmetrisation) = 0;
};

}
