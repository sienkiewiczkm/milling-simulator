#include "IntersectionCurve.hpp"

namespace fw
{

IntersectionCurve::IntersectionCurve():
    _isLoopBackEnabled{false},
    _loopBackDistance{},
    _minIndexDiff{}
{
}

IntersectionCurve::~IntersectionCurve()
{
}

void IntersectionCurve::setLooping(bool loopAllowed)
{
    _isLoopBackEnabled = loopAllowed;
}

void IntersectionCurve::setLoopbackMinimumIndexDifference(int minIndexDiff)
{
    _minIndexDiff = minIndexDiff;
}

void IntersectionCurve::setLoopBackDistance(double loopBackDistance)
{
    _loopBackDistance = loopBackDistance;
}

IntersectionCurveAddingResult IntersectionCurve::addCurvePoint(
    const ParametricSurfaceIntersection& intersection
)
{
    if (_isLoopBackEnabled)
    {
        auto connectableIndex = findConnectablePoint(intersection);
        if (connectableIndex >= 0)
        {
            _curvePoints.push_back(_curvePoints[connectableIndex]);
            return IntersectionCurveAddingResult::LoopedBack;
        }
    }

    _curvePoints.push_back(intersection);
    return IntersectionCurveAddingResult::Success;
}

const std::vector<ParametricSurfaceIntersection>
        IntersectionCurve::getCurvePoints() const
{
    return _curvePoints;
}

void IntersectionCurve::reverse()
{
    std::reverse(std::begin(_curvePoints), std::end(_curvePoints));
}

int IntersectionCurve::findConnectablePoint(
    const ParametricSurfaceIntersection& intersection
) const
{
    if (_curvePoints.size() < _minIndexDiff) { return -1; }

    for (auto i = 0; i < _curvePoints.size() - _minIndexDiff; ++i)
    {
        auto diffVector =
            _curvePoints[i].scenePosition - intersection.scenePosition;

        if (glm::length(diffVector) < _loopBackDistance)
        {
            return i;
        }
    }

    return -1;
}

}
