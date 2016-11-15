#include "SienkiewiczkModelIntersectionsHandler.hpp"
#include "ContourMerger.hpp"
#include "CurveSimplifier.hpp"
#include "GeometricIntersections.hpp"
#include <algorithm>
#include <iostream>

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
    auto handleInner =
        _intersectionFinder.intersect(_base, _handle, handleHole1Inside);
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

    auto handleOuterContours = fixParametricContour(extractRhs(handleOuter));
    auto handleInnerContours = fixParametricContour(extractRhs(handleInner));
    auto fixedHandleUpper = fixParametricContour(extractRhs(bodyUpperHandle));
    auto fixedHandleLower = fixParametricContour(extractRhs(bodyLowerHandle));

    auto bodyParametricBodyBackCont = fixParametricContour(
        extractRhs(bodyBack)
    );
    auto bodyParametricBodyFrontCont = fixParametricContour(
        extractRhs(bodyFront)
    );
    auto contBodyDrill = fixParametricContour(extractLhs(bodyDrill));
    auto contBodyUpperHandle = fixParametricContour(
        extractLhs(bodyUpperHandle)
    );
    auto contBodyLowerHandle = fixParametricContour(
        extractLhs(bodyLowerHandle)
    );

    prepareBodyTrimmedArea(
        bodyParametricBodyBackCont,
        bodyParametricBodyFrontCont,
        contBodyDrill,
        contBodyUpperHandle,
        contBodyLowerHandle
    );

    prepareHandleTrimmedArea(
        handleOuterContours,
        handleInnerContours,
        fixedHandleUpper,
        fixedHandleLower
    );

    auto drillParametricCont  = fixParametricContour(extractRhs(drillContour));
    auto drillBodyParametricContour =
        fixParametricContour(extractRhs(bodyDrill));

    prepareDrillTrimmedArea(drillParametricCont, drillBodyParametricContour);

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
    makeRenderable(handleInner);
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

void SienkiewiczkModelIntersectionsHandler::prepareBodyTrimmedArea(
    std::vector<std::vector<glm::dvec2>> backContour,
    std::vector<std::vector<glm::dvec2>> frontContour,
    std::vector<std::vector<glm::dvec2>> drillHole,
    std::vector<std::vector<glm::dvec2>> handleUpperHole,
    std::vector<std::vector<glm::dvec2>> handleLowerHole
)
{
    /*
    _bodyParametricContours.insert(
        std::end(_bodyParametricContours),
        std::begin(backContour),
        std::end(backContour)
    );

    _bodyParametricContours.insert(
        std::end(_bodyParametricContours),
        std::begin(frontContour),
        std::end(frontContour)
    );

    _bodyParametricContours.insert(
        std::end(_bodyParametricContours),
        std::begin(drillHole),
        std::end(drillHole)
    );

    _bodyParametricContours.insert(
        std::end(_bodyParametricContours),
        std::begin(handleUpperHole),
        std::end(handleUpperHole)
    );

    _bodyParametricContours.insert(
        std::end(_bodyParametricContours),
        std::begin(handleLowerHole),
        std::end(handleLowerHole)
    );
    */

    // hack: assume that zero index is all we want
    ContourMerger merger;
    _bodyParametricContours.push_back(backContour[0]);
    _bodyParametricContours.push_back(handleUpperHole[0]);

    auto temp = merger.merge2D(backContour[0], handleUpperHole[0]);
    //auto mergedContour = merger.merge2D(temp, handleLowerHole[0]);
    //_bodyParametricContours.push_back(temp);
}

void SienkiewiczkModelIntersectionsHandler::prepareDrillTrimmedArea(
    const std::vector<std::vector<glm::dvec2>>& visibleParametrizationContour,
    const std::vector<std::vector<glm::dvec2>>& bodyIntersectionContour
)
{
    _drillParametricContour.clear();

    auto drillBaseContour = visibleParametrizationContour[0];
    auto drillBaseContourRev = drillBaseContour;

    std::reverse(
        std::begin(drillBaseContourRev),
        std::end(drillBaseContourRev)
    );

    auto drillBodyContour = bodyIntersectionContour[0];
    auto drillBodyContourRev = drillBodyContour;

    std::reverse(
        std::begin(drillBodyContourRev),
        std::end(drillBodyContourRev)
    );

    auto cutBase = cutCurveUsingCurves(
        drillBaseContour,
        drillBodyContour,
        drillBodyContourRev
    );

    auto cutBody = cutCurveUsingCurves(
        drillBodyContour,
        drillBaseContour,
        drillBaseContourRev
    );

    std::vector<glm::dvec2> unifiedContour(cutBase);

    std::reverse_copy(
        std::begin(cutBody),
        std::end(cutBody),
        std::back_inserter(unifiedContour)
    );

    _drillParametricContour.push_back(unifiedContour);
}

void SienkiewiczkModelIntersectionsHandler::prepareHandleTrimmedArea(
    const std::vector<std::vector<glm::dvec2>>& handleOuterContours,
    const std::vector<std::vector<glm::dvec2>>& handleInnerContours,
    const std::vector<std::vector<glm::dvec2>>& handleUpperConnection,
    const std::vector<std::vector<glm::dvec2>>& handleLowerConnection
)
{
    _handleParametricContour.clear();

    auto trimmedInner = cutCurveUsingCurves(
        handleInnerContours[0],
        handleUpperConnection[0],
        handleLowerConnection[0]
    );

    auto trimmedOuter = cutCurveUsingCurves(
        handleOuterContours[0],
        handleLowerConnection[0],
        handleUpperConnection[0]
    );

    auto trimmedLower = cutCurveUsingCurves(
        handleLowerConnection[0],
        handleOuterContours[0],
        handleInnerContours[0]
    );

    auto trimmedUpper = cutCurveUsingCurves(
        handleUpperConnection[0],
        handleInnerContours[0],
        handleOuterContours[0]
    );

    std::vector<glm::dvec2> unifiedContour(trimmedInner);

    std::reverse_copy(
        std::begin(trimmedLower),
        std::end(trimmedLower),
        std::back_inserter(unifiedContour)
    );

    std::copy(
        std::begin(trimmedOuter),
        std::end(trimmedOuter),
        std::back_inserter(unifiedContour)
    );

    std::reverse_copy(
        std::begin(trimmedUpper),
        std::end(trimmedUpper),
        std::back_inserter(unifiedContour)
    );

    _handleParametricContour.push_back(unifiedContour);
}

std::vector<glm::dvec2>
        SienkiewiczkModelIntersectionsHandler::cutCurveUsingCurves(
    const std::vector<glm::dvec2>& input,
    const std::vector<glm::dvec2>& lhs,
    const std::vector<glm::dvec2>& rhs
)
{
    std::cout << "Cutting curve..." << std::endl;
    auto lhsInters = findAnyCurveIntersection(input, lhs);
    auto rhsInters = findAnyCurveIntersection(input, rhs);

    auto firstInt = lhsInters.first;
    auto secondInt = rhsInters.first;

    std::cout << "Found cut points: " << firstInt << " + " << secondInt
        << std::endl;

    if (firstInt == -1 && secondInt == -1) { return input; }

    if (secondInt == -1) secondInt = input.size() - 1;
    if (firstInt > secondInt)
    {
        std::swap(firstInt, secondInt);
    }

    std::cout << "Cutting segment:" << firstInt << " + " << secondInt
        << std::endl;

    std::vector<glm::dvec2> output;

    if (firstInt >= 0)
    {
        auto pt = getIntersectionPoint(
            input[lhsInters.first],
            input[lhsInters.first+1],
            lhs[lhsInters.second],
            lhs[lhsInters.second+1]
        );

        output.push_back(pt);
    }

    std::copy(
        std::begin(input) + firstInt + 1,
        std::begin(input) + secondInt,
        std::back_inserter(output)
    );

    if (secondInt + 1 < input.size())
    {
        auto pt = getIntersectionPoint(
            input[rhsInters.first],
            input[rhsInters.first+1],
            rhs[rhsInters.second],
            rhs[rhsInters.second+1]
        );

        output.push_back(pt);
    }

    return output;
}

glm::dvec2 SienkiewiczkModelIntersectionsHandler::getIntersectionPoint(
    const glm::dvec2& aStart,
    const glm::dvec2& aEnd,
    const glm::dvec2& bStart,
    const glm::dvec2& bEnd
)
{
    auto intersection = fw::intersectSegments<glm::dvec2, double>(
        aStart,
        aEnd,
        bStart,
        bEnd
    );

    if (intersection.kind != fw::GeometricIntersectionKind::Single)
    {
        return aStart;
    }

    return aStart + (aEnd - aStart) * intersection.singleIntersectionParameter;
}

std::pair<int, int>
        SienkiewiczkModelIntersectionsHandler::findAnyCurveIntersection(
    const std::vector<glm::dvec2>& lhs,
    const std::vector<glm::dvec2>& rhs
)
{
    for (auto j = 0; j+1 < rhs.size(); ++j)
    {
        for (auto i = 0; i+1 < lhs.size(); ++i)
        {
            auto intersection = fw::intersectSegments<glm::dvec2, double>(
                lhs[i],
                lhs[i+1],
                rhs[j],
                rhs[j+1]
            );

            if (intersection.kind == fw::GeometricIntersectionKind::Single)
            {
                return {i, j};
            }
        }
    }

    return {-1, -1};
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

std::vector<std::vector<glm::dvec2>>
        SienkiewiczkModelIntersectionsHandler::fixParametricContour(
    const std::vector<glm::dvec2> &contour
)
{
    fw::CurveSimplifier<glm::dvec2, double> simplifier;
    simplifier.setMinimumMergeCosine(0.9999);

    std::vector<std::vector<glm::dvec2>> output;

    // find crossing points
    std::vector<int> splitIndex;
    for (auto i = 0; (i+1) < contour.size(); ++i)
    {
        if (std::abs(contour[i].y - contour[i+1].y) > 0.3)
        {
            std::cout << i + 1 << std::endl;
            splitIndex.push_back(i + 1);
        }
    }

    if (splitIndex.size() == 0)
        return {simplifier.simplify(contour)};


    std::sort(std::begin(splitIndex), std::end(splitIndex));
    auto copied = 0;
    auto splitsUsed = 0;
    std::vector<glm::dvec2> subcontour;
    while (copied < contour.size())
    {
        auto index = (splitIndex[0] + copied) % contour.size();

        if (splitsUsed < splitIndex.size() && splitIndex[splitsUsed] == index)
        {
            // todo: it may be necessary to join the border in intersection pt
            ++splitsUsed;
            if (subcontour.size() > 1)
            {
                output.push_back(simplifier.simplify(subcontour));
            }
            subcontour.clear();
        }

        subcontour.push_back(contour[index]);
        ++copied;
    }

    if (subcontour.size() > 1)
    {
        output.push_back(simplifier.simplify(subcontour));
    }

    return output;
}

std::vector<glm::dvec2> SienkiewiczkModelIntersectionsHandler::extractRhs(
    const std::vector<fw::ParametricSurfaceIntersection>& intersection
)
{
    std::vector<glm::dvec2> output;
    for (auto& el: intersection)
    {
        output.push_back(el.rhsParameters);
    }
    return output;
}

std::vector<glm::dvec2> SienkiewiczkModelIntersectionsHandler::extractLhs(
    const std::vector<fw::ParametricSurfaceIntersection>& intersection
)
{
    std::vector<glm::dvec2> output;
    for (auto& el: intersection)
    {
        output.push_back(el.lhsParameters);
    }
    return output;
}

std::vector<std::vector<glm::dvec2>>
        SienkiewiczkModelIntersectionsHandler::getDrillParametricContours()
{
    return _drillParametricContour;
}

std::vector<std::vector<glm::dvec2>>
        SienkiewiczkModelIntersectionsHandler::getHandleParametricContours()
{
    return _handleParametricContour;
}

std::vector<std::vector<glm::dvec2>>
        SienkiewiczkModelIntersectionsHandler::getBodyParametricContours()
{
    return _bodyParametricContours;
}

}
