#include "BsplineSurface.hpp"
#include "BsplineEquidistantKnotGenerator.hpp"
#include "BsplineBasisEvaluator.hpp"

#include <iostream>

namespace fw
{

BsplineSurface::BsplineSurface(
    int surfaceDegree,
    glm::ivec2 controlPointsGridSize,
    std::vector<glm::dvec3> controlPoints,
    std::vector<double> knotsU,
    std::vector<double> knotsV,
    SurfaceFoldingMode foldingMode,
    int foldDepth
):
    _degree{surfaceDegree},
    _controlPointsGridSize{controlPointsGridSize},
    _controlPoints{controlPoints},
    _knotsU{knotsU},
    _knotsV{knotsV},
    _foldingMode{foldingMode},
    _foldDepth{foldDepth}
{
}

BsplineSurface::~BsplineSurface()
{
}

std::shared_ptr<ICurve3d> BsplineSurface::getConstParameterCurve(
    ParametrizationAxis constParameter,
    double parameter
) const
{
    auto numSubcurves = constParameter == ParametrizationAxis::U
        ? _controlPointsGridSize.y +
            (_foldingMode == SurfaceFoldingMode::ContinuousV ? _foldDepth : 0)
        : _controlPointsGridSize.x +
            (_foldingMode == SurfaceFoldingMode::ContinuousU ? _foldDepth : 0);

    auto foldBoundary = constParameter == ParametrizationAxis::U
        ? _controlPointsGridSize.y
        : _controlPointsGridSize.x;

    std::vector<glm::dvec3> subcurveControlPoints;
    for (auto i = 0; i < numSubcurves; ++i)
    {
        auto subcontrol = evaluateConstParamControlPoints(
            i % foldBoundary,
            constParameter
        );

        BsplineCurve3d curve {
            _degree,
            subcontrol,
            constParameter == ParametrizationAxis::U
                ? _knotsU
                : _knotsV
        };

        subcurveControlPoints.push_back(
            curve.evaluate(parameter)
        );
    }

    const auto &secondKnotVector = constParameter == ParametrizationAxis::U
        ? _knotsV
        : _knotsU;

    return std::static_pointer_cast<ICurve3d>(
        std::make_shared<BsplineCurve3d>(
            _degree,
            subcurveControlPoints,
            secondKnotVector
        )
    );
}

glm::dvec3 BsplineSurface::getPosition(glm::dvec2 parametrization) const
{
    return getConstParameterCurve(ParametrizationAxis::U, parametrization.x)
        ->evaluate(parametrization.y);
}

glm::dvec3 BsplineSurface::getNormal(glm::dvec2 parametrisation) const
{
    auto du = getDerivativeU(parametrisation);
    auto dv = getDerivativeV(parametrisation);
    return glm::normalize(glm::cross(dv, du));
}

glm::dvec3 BsplineSurface::getDerivativeU(glm::dvec2 parametrisation) const
{
    return getConstParameterCurve(ParametrizationAxis::V, parametrisation.y)
        ->getDerivativeCurve()->evaluate(parametrisation.x);
}

glm::dvec3 BsplineSurface::getDerivativeV(glm::dvec2 parametrisation) const
{
    return getConstParameterCurve(ParametrizationAxis::U, parametrisation.x)
        ->getDerivativeCurve()->evaluate(parametrisation.y);
}

int BsplineSurface::getDegree() const
{
    return _degree;
}

const std::vector<glm::dvec3> &BsplineSurface::getControlPoints() const
{
    return _controlPoints;
}

const std::vector<double> &BsplineSurface::getKnotsOnU() const
{
    return _knotsU;
}

const std::vector<double> &BsplineSurface::getKnotsOnV() const
{
    return _knotsV;
}

std::vector<glm::dvec3> BsplineSurface::evaluateConstParamControlPoints(
    int rowOrColumn,
    ParametrizationAxis constDirection
) const
{
    glm::ivec2 startPosition = constDirection == ParametrizationAxis::U
        ? glm::ivec2(0, rowOrColumn)
        : glm::ivec2(rowOrColumn, 0);

    glm::ivec2 increase = constDirection == ParametrizationAxis::U
        ? glm::ivec2(1, 0)
        : glm::ivec2(0, 1);

    auto xFoldAdditional = _foldingMode == SurfaceFoldingMode::ContinuousU
        ? _foldDepth
        : 0;

    auto yFoldAdditional = _foldingMode == SurfaceFoldingMode::ContinuousV
        ? _foldDepth
        : 0;

    auto stepLimit = constDirection == ParametrizationAxis::U
        ? _controlPointsGridSize.x + xFoldAdditional
        : _controlPointsGridSize.y + yFoldAdditional;

    std::vector<glm::dvec3> subcontrolPoints;

    auto pt = startPosition;

    for (auto i = 0; i < stepLimit; ++i)
    {
        auto x = pt.x % _controlPointsGridSize.x;
        auto y = pt.y % _controlPointsGridSize.y;

        subcontrolPoints.push_back(
            _controlPoints[_controlPointsGridSize.x * y + x]
        );

        pt += increase;
    }

    return subcontrolPoints;
}


}
