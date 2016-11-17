#pragma once

#include "BsplineSurface.hpp"

#include <memory>

namespace fw
{

class BsplineNonVanishingReparametrization:
    public IParametricSurfaceUV
{
public:
    BsplineNonVanishingReparametrization(
        std::shared_ptr<BsplineSurface> bsplineSurface
    );

    virtual ~BsplineNonVanishingReparametrization();

    virtual std::shared_ptr<ICurve3d> getConstParameterCurve(
        ParametrizationAxis constParameter,
        double parameter
    ) const override;

    virtual glm::dvec3 getPosition(glm::dvec2 parametrization) const override;
    virtual glm::dvec3 getNormal(glm::dvec2 parmetrization) const override;
    virtual glm::dvec3 getDerivativeU(
        glm::dvec2 parametrisation
    ) const override;
    virtual glm::dvec3 getDerivativeV(
        glm::dvec2 parametrisation
    ) const override;

private:
    glm::dvec2 calculateReparametrizationDerivativeFactors() const;
    glm::dvec2 calculateReparametrization(glm::dvec2 parametrization) const;

    std::shared_ptr<BsplineSurface> _bsplineSurface;
};

}
