#include "SurfaceIntersectionNewtonIterable.hpp"
#include <iostream>
#include <cmath>

#include <glm/ext.hpp>

namespace fw
{

SurfaceIntersectionNewtonIterable::SurfaceIntersectionNewtonIterable():
    _covergenceThreshold{0.005}
{
}

void SurfaceIntersectionNewtonIterable::setSurfaces(
    const std::shared_ptr<IParametricSurfaceUV> lhs,
    const std::shared_ptr<IParametricSurfaceUV> rhs
)
{
    _lhsSurface = lhs;
    _rhsSurface = rhs;
}

void SurfaceIntersectionNewtonIterable::setTangentVector(
    const glm::dvec3& tangentVector
)
{
    _tangentVector = glm::normalize(tangentVector);
}

double SurfaceIntersectionNewtonIterable::getCovergenceThreshold() const
{
    return _covergenceThreshold;
}

void SurfaceIntersectionNewtonIterable::setCovergenceThreshold(double threshold)
{
    _covergenceThreshold = threshold;
}

void SurfaceIntersectionNewtonIterable::setPlaneDistance(double planeDistance)
{
    _planeDistance = planeDistance;
}

void SurfaceIntersectionNewtonIterable::setInitialParameters(
    const glm::dvec4& initialParameters
)
{
    _initialParameters = initialParameters;
    _initialLhsPosition = _lhsSurface->getPosition(
        {_initialParameters.x, _initialParameters.y}
    );
}

void SurfaceIntersectionNewtonIterable::setCurrentParameters(
    const glm::dvec4& parameters
)
{
    _parameters = parameters;
}

bool SurfaceIntersectionNewtonIterable::areParametersValid(
    const glm::dvec4& parameters
) const
{
    bool uOutsideOfBounds = parameters.x < 0 || parameters.x > 1
        || parameters.z < 0 || parameters.z > 1;
    bool anyFieldIsNaN = std::isnan(parameters.x)
        || std::isnan(parameters.y)
        || std::isnan(parameters.z)
        || std::isnan(parameters.w);
    return !uOutsideOfBounds && !anyFieldIsNaN;
}

bool SurfaceIntersectionNewtonIterable::hasParameterConverged(
    const glm::dvec4& current
) const
{
    return hasSingleParameterConverged(
        {_parameters.x, _parameters.y},
        {current.x, current.y}
    ) && hasSingleParameterConverged(
        {_parameters.z, _parameters.w},
        {current.z, current.w}
    );
}

glm::dvec4 SurfaceIntersectionNewtonIterable::evaluateFunction() const
{
    // Equation system:
    //  P(u,v) - Q(s,t) = 0
    //  <P(u,v) - P0, t> - d = 0
    auto lhsPosition = _lhsSurface->getPosition({_parameters.x, _parameters.y});
    auto rhsPosition = _rhsSurface->getPosition({_parameters.z, _parameters.w});
    auto firstEquation = lhsPosition - rhsPosition;
    auto secondEquation = calculateTangentPlaneDistance(lhsPosition);

    glm::dvec4 functionValue{
        firstEquation.x,
        firstEquation.y,
        firstEquation.z,
        secondEquation
    };

    return functionValue;
}

glm::dmat4 SurfaceIntersectionNewtonIterable::getJacobianInverse() const
{
    glm::dvec2 lhsParam{_parameters.x, _parameters.y};
    glm::dvec2 rhsParam{_parameters.z, _parameters.w};

    auto lhsdU = _lhsSurface->getDerivativeU(lhsParam);
    auto lhsdV = _lhsSurface->getDerivativeV(lhsParam);

    auto rhsdU = _rhsSurface->getDerivativeU(rhsParam);
    auto rhsdV = _rhsSurface->getDerivativeV(rhsParam);

    auto planeDerivatives = getTangentPlaneDerivatives();

    glm::dmat4 jacobian{
        {  lhsdU.x,  lhsdU.y,  lhsdU.z, planeDerivatives.x }, // dx
        {  lhsdV.x,  lhsdV.y,  lhsdV.z, planeDerivatives.y }, // dy
        { -rhsdU.x, -rhsdU.y, -rhsdU.z, planeDerivatives.z }, // dz
        { -rhsdV.x, -rhsdV.y, -rhsdV.z, planeDerivatives.w }  // dw
    };

    return glm::inverse(jacobian);
}

glm::dvec4 SurfaceIntersectionNewtonIterable::correctParametrisation(
    const glm::dvec4& parameters
) const
{
    // hack: skipped interogation - all model parts are folded on v
    // todo: introduce cyclic domains for surfaces and interrogation
    auto fixedParameter = parameters;

    fixedParameter.y = fixedParameter.y - static_cast<int>(fixedParameter.y);
    if (fixedParameter.y < 0) fixedParameter.y += 1.0;

    fixedParameter.w = fixedParameter.w - static_cast<int>(fixedParameter.w);
    if (fixedParameter.w < 0) fixedParameter.w += 1.0;

    return fixedParameter;
}

double SurfaceIntersectionNewtonIterable::calculateTangentPlaneDistance(
    const glm::dvec3& lhsPosition
) const
{
    return glm::dot(lhsPosition - _initialLhsPosition, _tangentVector)
        - _planeDistance;
}

glm::dvec4 SurfaceIntersectionNewtonIterable::getTangentPlaneDerivatives() const
{
    const double dh = 10e-6;
    glm::dvec2 lhsParams{_parameters.x, _parameters.y};

    auto du = calculateTangentPlaneSingleDerivative(lhsParams, {1, 0}, dh);
    auto dv = calculateTangentPlaneSingleDerivative(lhsParams, {0, 1}, dh);

    return {du, dv, 0, 0};
}

double SurfaceIntersectionNewtonIterable::calculateTangentPlaneSingleDerivative(
    const glm::dvec2 &lhsParams,
    const glm::dvec2& direction,
    const double delta
) const
{
    auto hMinusTwo = calculateTangentPlaneDistance(
        _lhsSurface->getPosition(lhsParams - 2.0 * direction * delta)
    );
    auto hMinusOne = calculateTangentPlaneDistance(
        _lhsSurface->getPosition(lhsParams - 1.0 * direction * delta)
    );
    auto hPlusOne = calculateTangentPlaneDistance(
        _lhsSurface->getPosition(lhsParams + 1.0 * direction * delta)
    );
    auto hPlusTwo = calculateTangentPlaneDistance(
        _lhsSurface->getPosition(lhsParams + 2.0 * direction * delta)
    );

    auto numericDerivative =
        (hMinusTwo - 8.0 * hMinusOne + 8.0 * hPlusOne - hPlusTwo)
            / (12 * delta);

    return numericDerivative;
}

bool SurfaceIntersectionNewtonIterable::hasSingleParameterConverged(
    const glm::dvec2& previous,
    const glm::dvec2& current
) const
{
    return glm::length(previous - current) < _covergenceThreshold;
}

}
