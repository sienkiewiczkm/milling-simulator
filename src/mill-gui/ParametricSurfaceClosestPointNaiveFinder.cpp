#include "ParametricSurfaceClosestPointNaiveFinder.hpp"
#include <limits>

namespace fw
{

ParametricSurfaceClosestPointNaiveFinder::
        ParametricSurfaceClosestPointNaiveFinder():
    _referencePoint{},
    _samplingResolution{}
{
}

ParametricSurfaceClosestPointNaiveFinder::
        ~ParametricSurfaceClosestPointNaiveFinder()
{
}

void ParametricSurfaceClosestPointNaiveFinder::
        setReferencePoint(glm::dvec3 referencePoint)
{
    _referencePoint = referencePoint;
}

void ParametricSurfaceClosestPointNaiveFinder::
        setSamplingResolution(glm::ivec2 samplingResolution)
{
    _samplingResolution = samplingResolution;
}

glm::dvec2 ParametricSurfaceClosestPointNaiveFinder::
        find(const IParametricSurfaceUV& surface)
{
    double closestDistance = std::numeric_limits<double>::max();
    glm::dvec2 closestPoint{};

    for (auto x = 0; x < _samplingResolution.x; ++x)
    {
        auto dx = x / static_cast<double>(_samplingResolution.x);
        auto curve = surface.getConstParameterCurve(ParametrizationAxis::U, dx);
        for (auto y = 0; y < _samplingResolution.y; ++y)
        {
            auto dy = y / static_cast<double>(_samplingResolution.y);
            auto position = curve->evaluate(dy);
            auto distance = glm::length(position - _referencePoint);
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestPoint = glm::dvec2{dx, dy};
            }
        }
    }

    return closestPoint;
}

}
