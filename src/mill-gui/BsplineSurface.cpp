#include "BsplineSurface.hpp"
#include "BsplineEquidistantKnotGenerator.hpp"
#include "BsplineBasisEvaluator.hpp"

namespace fw
{

BsplineSurface::BsplineSurface(
    int surfaceDegree,
    glm::ivec2 controlPointsGridSize,
    std::vector<glm::dvec3> controlPoints,
    std::shared_ptr<IBsplineKnotGenerator> knotGenerator,
    SurfaceFoldingMode foldingMode
):
    _degree{surfaceDegree},
    _controlPointsGridSize{controlPointsGridSize},
    _controlPoints{controlPoints},
    _knotGenerator{knotGenerator},
    _foldingMode{foldingMode}
{
    auto xFoldAdditional = _foldingMode == SurfaceFoldingMode::ContinuousU
        ? _degree
        : 0;

    auto yFoldAdditional = _foldingMode == SurfaceFoldingMode::ContinuousV
        ? _degree
        : 0;

    _knotsX = _knotGenerator->generate(
        _controlPointsGridSize.x + xFoldAdditional,
        _degree
    );

    _knotsY = _knotGenerator->generate(
        _controlPointsGridSize.y + yFoldAdditional,
        _degree
    );
}

BsplineSurface::~BsplineSurface()
{
}

glm::dvec3 BsplineSurface::getPosition(glm::dvec2 parametrisation)
{
    // todo: remove evaluation mode - no differences
    auto evaluationMode = EvaluationDirection::AlongXAxis;
    auto numSubcurves = _controlPointsGridSize.x;

    std::vector<glm::dvec3> subcurveControlPoints;
    for (auto i = 0; i < numSubcurves; ++i)
    {
        auto subcontrol = evaluateSubcontrolPoints(i, evaluationMode);
        auto point = evaluateCurve(subcontrol, parametrisation.x);
        subcurveControlPoints.push_back(point);
    }

    if (_foldingMode == SurfaceFoldingMode::ContinuousV)
    {
        for (auto i = 0; i < _degree; ++i)
        {
            subcurveControlPoints.push_back(subcurveControlPoints[i]);
        }
    }

    return evaluateCurve(subcurveControlPoints, parametrisation.y);
}

glm::dvec3 BsplineSurface::getNormal(glm::dvec2 parmetrisation)
{
    return glm::dvec3();
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
    return _knotsX;
}

const std::vector<double> &BsplineSurface::getKnotsOnV() const
{
    return _knotsY;
}

std::vector<glm::dvec3> BsplineSurface::evaluateSubcontrolPoints(
    int rowOrColumn,
    EvaluationDirection direction
) const
{
    glm::ivec2 startPosition = direction == EvaluationDirection::AlongXAxis
        ? glm::ivec2(0, rowOrColumn)
        : glm::ivec2(rowOrColumn, 0);

    glm::ivec2 increase = direction == EvaluationDirection::AlongXAxis
        ? glm::ivec2(1, 0)
        : glm::ivec2(0, 1);

    auto xFoldAdditional = _foldingMode == SurfaceFoldingMode::ContinuousU
        ? _degree
        : 0;

    auto yFoldAdditional = _foldingMode == SurfaceFoldingMode::ContinuousV
        ? _degree
        : 0;

    auto stepLimit = direction == EvaluationDirection::AlongXAxis
        ? _controlPointsGridSize.x + xFoldAdditional
        : _controlPointsGridSize.y + yFoldAdditional;

    std::vector<glm::dvec3> subcontrolPoints;

    auto pt = startPosition;

    for (auto i = 0; i < stepLimit; ++i)
    {
        auto x = pt.x % _controlPointsGridSize.x;
        auto y = pt.y % _controlPointsGridSize.y;

        subcontrolPoints.push_back(
            _controlPoints[_controlPointsGridSize.y * y + x]
        );
        
        pt += increase;
    }

    return subcontrolPoints;
}

glm::dvec3 BsplineSurface::evaluateCurve(
    const std::vector<glm::dvec3> &controlPoints,
    double parameter
)
{
    auto knots = _knotGenerator->generate(controlPoints.size(), _degree);
    auto basisEvaluator = std::make_shared<fw::BsplineBasisEvaluator>(
        _degree,
        knots
    );

    auto linearSolver = 
        std::make_shared<fw::LinearCombinationEvaluator<glm::dvec3>>(
            basisEvaluator,
            controlPoints
        );

    return linearSolver->evaluate(parameter);
}

}
