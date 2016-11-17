#pragma once

#include "ICurve.hpp"

#include <glm/glm.hpp>

#include <memory>

namespace fw
{

enum class ParametrizationAxis
{
    U,
    V
};

class IParametricSurfaceUV
{
public:
    virtual ~IParametricSurfaceUV() = default;

    virtual std::shared_ptr<ICurve3d> getConstParameterCurve(
        ParametrizationAxis constParameter,
        double parameter
    ) const = 0;

    virtual glm::dvec3 getPosition(glm::dvec2 parametrisation) const = 0;
    virtual glm::dvec3 getNormal(glm::dvec2 parmetrisation) const = 0;
    virtual glm::dvec3 getDerivativeU(glm::dvec2 parametrisation) const = 0;
    virtual glm::dvec3 getDerivativeV(glm::dvec2 parametrisation) const = 0;
};

}
