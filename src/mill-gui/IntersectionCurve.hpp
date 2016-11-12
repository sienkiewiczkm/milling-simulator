#pragma once
#include "ParametricSurfaceIntersection.hpp"
#include <vector>

namespace fw
{

enum class IntersectionCurveAddingResult
{
    Unknown,
    Success,
    LoopedBack
};

class IntersectionCurve
{
public:
    IntersectionCurve();
    ~IntersectionCurve();

    void setLooping(bool loopAllowed);
    void setLoopbackMinimumIndexDifference(int minIndexDiff);
    void setLoopBackDistance(double loopBackDistance);

    IntersectionCurveAddingResult addCurvePoint(
        const ParametricSurfaceIntersection& intersection
    );

    void reverse();

    const std::vector<ParametricSurfaceIntersection> getCurvePoints() const;

protected:
    int findConnectablePoint(
        const ParametricSurfaceIntersection& intersection
    ) const;

private:
    bool _isLoopBackEnabled;
    int _minIndexDiff;
    double _loopBackDistance;
    std::vector<ParametricSurfaceIntersection> _curvePoints;
};

}
