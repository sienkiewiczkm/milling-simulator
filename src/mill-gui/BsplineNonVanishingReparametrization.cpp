#include "BsplineNonVanishingReparametrization.hpp"
#include "BsplineNonVanishingCurveReparametrization.hpp"

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
    auto surfaceReparametrization =
        calculateReparametrization({parameter, parameter});

    auto bsplineCurve = _bsplineSurface->getConstParameterCurve(
        constParameter,
        constParameter == ParametrizationAxis::U
            ? surfaceReparametrization.x
            : surfaceReparametrization.y
    );

    return std::make_shared<BsplineNonVanishingCurveReparametrization3d>(
        std::static_pointer_cast<BsplineCurve3d>(bsplineCurve)
    );
}

glm::dvec3 BsplineNonVanishingReparametrization::getPosition(
    glm::dvec2 parametrization
) const
{
    return getConstParameterCurve(ParametrizationAxis::U, parametrization.x)
        ->evaluate(parametrization.y);
}

glm::dvec3 BsplineNonVanishingReparametrization::getNormal(
    glm::dvec2 parametrisation
) const
{
    auto du = getDerivativeU(parametrisation);
    auto dv = getDerivativeV(parametrisation);
    return glm::normalize(glm::cross(dv, du));
}

glm::dvec3 BsplineNonVanishingReparametrization::getDerivativeU(
    glm::dvec2 parametrisation
) const
{
    auto derivFactors = calculateReparametrizationDerivativeFactors();
    return derivFactors.y *
        getConstParameterCurve(ParametrizationAxis::V, parametrisation.y)
            ->getDerivativeCurve()->evaluate(parametrisation.x);
}

glm::dvec3 BsplineNonVanishingReparametrization::getDerivativeV(
    glm::dvec2 parametrisation
) const
{
    auto derivFactors = calculateReparametrizationDerivativeFactors();
    return derivFactors.x *
        getConstParameterCurve(ParametrizationAxis::U, parametrisation.x)
            ->getDerivativeCurve()->evaluate(parametrisation.y);
}

glm::dvec2 BsplineNonVanishingReparametrization::
        calculateReparametrizationDerivativeFactors() const
{
    auto knotsX = _bsplineSurface->getKnotsOnU();
    auto knotsY = _bsplineSurface->getKnotsOnV();
    auto degree = _bsplineSurface->getDegree();
    auto duFactor = knotsX[knotsX.size() - degree - 1] - knotsX[degree];
    auto dvFactor = knotsY[knotsY.size() - degree - 1] - knotsY[degree];
    return {duFactor, dvFactor};
}

glm::dvec2 BsplineNonVanishingReparametrization::calculateReparametrization(
    glm::dvec2 parametrization
) const
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
