#include "BsplineSurface.hpp"
#include "BsplineEquidistantKnotGenerator.hpp"
#include "BsplineBasisEvaluator.hpp"

namespace fw
{

BsplineSurface::BsplineSurface(
    int surfaceDegree,
    glm::ivec2 controlPointsGridSize,
    std::vector<glm::dvec3> controlPoints,
    std::shared_ptr<IBsplineKnotGenerator> knotGenerator
):
    _degree{surfaceDegree},
    _controlPointsGridSize{controlPointsGridSize},
    _controlPoints{controlPoints},
    _knotGenerator{knotGenerator}
{
    _knotsX = _knotGenerator->generate(_controlPointsGridSize.x, _degree);
    _knotsY = _knotGenerator->generate(_controlPointsGridSize.y, _degree);
}

BsplineSurface::~BsplineSurface()
{
}

glm::dvec3 BsplineSurface::getPosition(glm::dvec2 parametrisation)
{
    auto evaluationMode = EvaluationDirection::AlongXAxis;
    auto numSubcurves = evaluationMode == EvaluationDirection::AlongXAxis
        ? _controlPointsGridSize.x
        : _controlPointsGridSize.y;

    if (evaluationMode == EvaluationDirection::AlongYAxis)
    {
        parametrisation = glm::dvec2(parametrisation.y, parametrisation.x);
    }

    std::vector<glm::dvec3> subcurveControlPoints;
    for (auto i = 0; i < numSubcurves; ++i)
    {
        auto subcontrol = evaluateSubcontrolPoints(i, evaluationMode);
        auto point = evaluateCurve(subcontrol, parametrisation.x);
        subcurveControlPoints.push_back(point);
    }

    return evaluateCurve(subcurveControlPoints, parametrisation.y);
}

glm::dvec3 BsplineSurface::getNormal(glm::dvec2 parmetrisation)
{
    return glm::dvec3();
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

    std::vector<glm::dvec3> subcontrolPoints;

    for (auto pt = startPosition;
         pt.x < _controlPointsGridSize.x && pt.y < _controlPointsGridSize.y;
         pt += increase
    )
    {
        subcontrolPoints.push_back(
            _controlPoints[_controlPointsGridSize.y * pt.y + pt.x]
        );
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
