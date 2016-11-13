#include "SienkiewiczkModelIntersectionsHandler.hpp"
#include "ContourMerger.hpp"
#include <algorithm>

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
    _base(base),
    _toolRadius{0.2}
{
}

SienkiewiczkModelIntersectionsHandler::~SienkiewiczkModelIntersectionsHandler()
{
}

void SienkiewiczkModelIntersectionsHandler::setScaledToolRadius(
    double toolRadius
)
{
    _toolRadius = toolRadius;
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
        _intersectionFinder.intersect(_base, _handle, handleTop);
    auto drillLowerPart =
        _intersectionFinder.intersect(_base, _drill, drillLowerCorner);
    auto drillUpperPart =
        _intersectionFinder.intersect(_base, _drill, drillUpperCorner);

    auto bodyDrill =
        _intersectionFinder.intersect(_body, _drill, drillLowerCorner);
    auto bodyUpperHandle =
        _intersectionFinder.intersect(_body, _handle, handleHole1Inside);
    auto bodyLowerHandle =
        _intersectionFinder.intersect(_body, _handle, handleHole2Outside);

    auto moveDistance = _toolRadius;
    auto bodyContour = joinIntersections(bodyBack, bodyFront);
    auto bodyToolControur = moveContourAlongFlattenedNormal(
        bodyContour,
        _body,
        ContourMoveParameter::RHS,
        moveDistance
    );

    auto drillContour = drillLowerPart;
    auto drillToolContour = moveContourAlongFlattenedNormal(
        drillContour,
        _drill,
        ContourMoveParameter::RHS,
        moveDistance
    );

    auto handleContour = handleOuter;
    auto handleToolContour = moveContourAlongFlattenedNormal(
        handleContour,
        _handle,
        ContourMoveParameter::RHS,
        moveDistance
    );

    ContourMerger contourMerger;
    auto contour = contourMerger.merge2D(bodyToolControur, handleToolContour);
    _objectShiftedContour = contourMerger.merge2D(contour, drillToolContour);
    makeRenderable(_objectShiftedContour);

    makeRenderable(bodyContour);
    makeRenderable(drillLowerPart);
    makeRenderable(handleContour);
    makeRenderable(bodyDrill);
    makeRenderable(bodyUpperHandle);
    makeRenderable(bodyLowerHandle);
}

std::vector<glm::dvec3>
    SienkiewiczkModelIntersectionsHandler::getObjectContour(
        glm::dmat4 objectTransformation
    )
{
    std::vector<glm::dvec3> output;

    std::transform(
        std::begin(_objectShiftedContour),
        std::end(_objectShiftedContour),
        std::back_inserter(output),
        [objectTransformation](const glm::dvec3 &pos)
        {
            return glm::dvec3{objectTransformation * glm::dvec4{pos, 1.0}};
        }
    );

    return output;
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

std::vector<glm::dvec3>
        SienkiewiczkModelIntersectionsHandler::moveContourAlongFlattenedNormal(
    const std::vector<fw::ParametricSurfaceIntersection>& intersection,
    std::shared_ptr<fw::IParametricSurfaceUV> surface,
    SienkiewiczkModelIntersectionsHandler::ContourMoveParameter moveParameter,
    double distance
)
{
    std::vector<glm::dvec3> output;

    for (const auto &param: intersection)
    {
        glm::dvec2 parameter = moveParameter == ContourMoveParameter::LHS
            ? param.lhsParameters
            : param.rhsParameters;

        auto normal = surface->getNormal(parameter);
        normal.z = 0.0;
        normal = glm::normalize(normal);

        auto moved = param.scenePosition + distance * normal;
        output.push_back(moved);
    }

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

void SienkiewiczkModelIntersectionsHandler::makeRenderable(
    const std::vector<glm::dvec3>& contour
)
{
    std::vector<fw::VertexColor> intersectionVertices;
    for (const auto &point: contour)
    {
        intersectionVertices.push_back({
            point, {1.0, 1.0, 1.0}
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
