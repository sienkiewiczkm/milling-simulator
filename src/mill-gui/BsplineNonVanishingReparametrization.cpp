#include "BsplineNonVanishingReparametrization.hpp"

namespace fw
{

BsplineNonVanishingReparametrization::BsplineNonVanishingReparametrization(
    std::shared_ptr<BsplineSurface> bsplineSurface
):
    _bsplineSurface{bsplineSurface}
{
}

BsplineNonVanishingReparametrization::~BsplineNonVanishingReparametrization()
{
}

std::shared_ptr<ICurve3d>
        BsplineNonVanishingReparametrization::getConstParameterCurve(
    ParametrizationAxis constParameter,
    double parameter
) const
{
    return _bsplineSurface->getConstParameterCurve(constParameter, parameter);
}

glm::dvec3 BsplineNonVanishingReparametrization::getPosition(
    glm::dvec2 parametrization
)
{
    auto reparametrization = calculateReparametrization(parametrization);
    return _bsplineSurface->getPosition(reparametrization);
}

glm::dvec3 BsplineNonVanishingReparametrization::getNormal(
    glm::dvec2 parametrization
)
{
    auto reparametrization = calculateReparametrization(parametrization);
    return _bsplineSurface->getNormal(reparametrization);
}

glm::dvec2 BsplineNonVanishingReparametrization::calculateReparametrization(
    glm::dvec2 parametrization
)
{
    auto knotsX = _bsplineSurface->getKnotsOnU();
    auto knotsY = _bsplineSurface->getKnotsOnV();
    auto degree = _bsplineSurface->getDegree();

    auto newU = glm::mix(
        knotsX[degree],
        knotsX[knotsX.size() - degree - 1],
        parametrization.x
    );

    auto newV = glm::mix(
        knotsY[degree],
        knotsY[knotsY.size() - degree - 1],
        parametrization.y
    );

    return glm::vec2 { newU, newV };
}

}
