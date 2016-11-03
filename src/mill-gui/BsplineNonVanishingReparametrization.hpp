#pragma once

#include "BsplineSurface.hpp"

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

    virtual glm::dvec3 getPosition(glm::dvec2 parametrization) override;
    virtual glm::dvec3 getNormal(glm::dvec2 parmetrization) override;

private:
    glm::dvec2 calculateReparametrization(glm::dvec2 parametrization);

    std::shared_ptr<BsplineSurface> _bsplineSurface;
};

}
