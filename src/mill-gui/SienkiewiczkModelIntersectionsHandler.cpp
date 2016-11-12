#include "SienkiewiczkModelIntersectionsHandler.hpp"

namespace ms
{

SienkiewiczkModelIntersectionsHandler::SienkiewiczkModelIntersectionsHandler(
    const std::shared_ptr<fw::IParametricSurfaceUV> body,
    const std::shared_ptr<fw::IParametricSurfaceUV> handle,
    const std::shared_ptr<fw::IParametricSurfaceUV> drill,
    const std::shared_ptr<fw::IParametricSurfaceUV> base
):
    _body(body),
    _handle(handle),
    _drill(drill),
    _base(base)
{
}

SienkiewiczkModelIntersectionsHandler::~SienkiewiczkModelIntersectionsHandler()
{
}

void SienkiewiczkModelIntersectionsHandler::findIntersections()
{
    glm::dvec3 bodyBackSidePoint{0.05, 1.8, 0.05};
    glm::dvec3 bodyFrontSidePoint{0.709, -0.069, -0.126};
    glm::dvec3 drillLowerCorner{-1.003, -0.367, -0.126};
    glm::dvec3 drillUpperCorner{-1.555, -0.246, -0.126};
    glm::dvec3 handleHole1Inside{0.005, 1.749, -0.126};
    glm::dvec3 handleHole2Outside{2.050, 1.758, -0.126};
    glm::dvec3 handleTop{0.900, 2.962, -0.126};

    auto bodyBack =
        _intersectionFinder.intersect(_base, _body, bodyBackSidePoint);
    auto bodyFront =
        _intersectionFinder.intersect(_base, _body, bodyFrontSidePoint);
    auto handleOuter =
        _intersectionFinder.intersect(_handle, _base, handleTop);
    auto drillLowerPart =
        _intersectionFinder.intersect(_base, _drill, drillLowerCorner);
    auto drillUpperPart =
        _intersectionFinder.intersect(_base, _drill, drillUpperCorner);

    makeRenderable(joinIntersections(bodyBack, bodyFront));
    makeRenderable(joinIntersections(drillLowerPart, drillUpperPart));
    makeRenderable(handleOuter);
}

std::vector<fw::ParametricSurfaceIntersection>
        SienkiewiczkModelIntersectionsHandler::joinIntersections(
    const std::vector<fw::ParametricSurfaceIntersection> &lhs,
    const std::vector<fw::ParametricSurfaceIntersection> &rhs
)
{
    std::vector<fw::ParametricSurfaceIntersection> output(lhs);

    auto firstDistance = glm::length(
        lhs.front().scenePosition - rhs.front().scenePosition
    );

    auto lastDistance = glm::length(
        lhs.front().scenePosition - rhs.back().scenePosition
    );

    if (lastDistance < firstDistance)
    {
        std::copy(
            std::begin(rhs),
            std::end(rhs),
            std::back_inserter(output)
        );
    }
    else
    {
        std::reverse_copy(
            std::begin(rhs),
            std::end(rhs),
            std::back_inserter(output)
        );
    }

    output.push_back(lhs.front());
    return output;
}

void SienkiewiczkModelIntersectionsHandler::makeRenderable(
    const std::vector<fw::ParametricSurfaceIntersection>& intersection
)
{
    std::vector<fw::VertexColor> intersectionVertices;
    for (const auto &point: intersection)
    {
        intersectionVertices.push_back({
            point.scenePosition, {1.0, 1.0, 1.0}
        });
    }

    auto intersectionLine = std::make_shared<fw::PolygonalLine>(
        intersectionVertices
    );

    _intersectionsRepresentations.push_back(intersectionLine);
}

void SienkiewiczkModelIntersectionsHandler::render() const
{
    for (const auto &representation: _intersectionsRepresentations)
    {
        representation->render();
    }
}

}
