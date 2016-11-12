#pragma once

#include "IParametricSurfaceUV.hpp"
#include "ParametricSurfaceIntersection.hpp"
#include "ParametricSurfaceIntersectionFinder.hpp"
#include "PolygonalLine.hpp"
#include <memory>
#include <vector>

namespace ms
{

class SienkiewiczkModelIntersectionsHandler
{
public:
    SienkiewiczkModelIntersectionsHandler(
        const std::shared_ptr<fw::IParametricSurfaceUV> body,
        const std::shared_ptr<fw::IParametricSurfaceUV> handle,
        const std::shared_ptr<fw::IParametricSurfaceUV> drill,
        const std::shared_ptr<fw::IParametricSurfaceUV> base
    );

    ~SienkiewiczkModelIntersectionsHandler();

    void findIntersections();
    void render() const;

protected:
    /// \brief Forces join of two intersections. It connects closest end-points
    //         of the intersection together making a circle.
    std::vector<fw::ParametricSurfaceIntersection> joinIntersections(
        const std::vector<fw::ParametricSurfaceIntersection>& lhs,
        const std::vector<fw::ParametricSurfaceIntersection>& rhs
    );

    enum class ContourMoveParameter
    {
        LHS,
        RHS,
    };

    std::vector<glm::dvec3> moveContourAlongFlattenedNormal(
        const std::vector<fw::ParametricSurfaceIntersection>& intersection,
        std::shared_ptr<fw::IParametricSurfaceUV> surface,
        ContourMoveParameter moveParameter,
        double distance
    );

    void makeRenderable(
        const std::vector<fw::ParametricSurfaceIntersection>& intersection
    );

    void makeRenderable(
        const std::vector<glm::dvec3>& contour
    );

private:
    fw::ParametricSurfaceIntersectionFinder _intersectionFinder;
    std::shared_ptr<fw::IParametricSurfaceUV> _body;
    std::shared_ptr<fw::IParametricSurfaceUV> _handle;
    std::shared_ptr<fw::IParametricSurfaceUV> _drill;
    std::shared_ptr<fw::IParametricSurfaceUV> _base;
    std::vector<std::shared_ptr<fw::PolygonalLine>>
        _intersectionsRepresentations;
};

}
