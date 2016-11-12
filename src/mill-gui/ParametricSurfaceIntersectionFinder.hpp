#pragma once

#include "IParametricSurfaceUV.hpp"
#include "ParametricSurfaceIntersection.hpp"
#include "IntersectionCurve.hpp"
#include "SurfaceIntersectionNewtonIterable.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace fw
{

class ParametricSurfaceIntersectionFinder
{
public:
    ParametricSurfaceIntersectionFinder();
    virtual ~ParametricSurfaceIntersectionFinder();

    std::vector<ParametricSurfaceIntersection> intersect(
        const std::shared_ptr<IParametricSurfaceUV> lhs,
        const std::shared_ptr<IParametricSurfaceUV> rhs,
        glm::vec3 neighbourhoodPoint
    );

protected:
    bool iterateInDirection(
        double tangentMultipler,
        glm::dvec2 initialLhsParams,
        glm::dvec2 initialRhsParams
    );

    std::shared_ptr<IParametricSurfaceUV> _lhs;
    std::shared_ptr<IParametricSurfaceUV> _rhs;

    SurfaceIntersectionNewtonIterator _newtonIterator;
    SurfaceIntersectionNewtonIterable _newtonIterable;
    IntersectionCurve _intersectionCurve;
};

}
