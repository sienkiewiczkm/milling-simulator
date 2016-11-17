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

    auto refineBodyDrill = bodyDrill;
    inplaceMoveContourAlongNormal(
        refineBodyDrill,
        _body,
        ContourMoveParameter::LHS,
        _toolRadius
    );
    inplaceMoveContourAlongNormal(
        refineBodyDrill,
        _drill,
        ContourMoveParameter::RHS,
        _toolRadius
    );

    auto refineBodyUpperHandle = bodyUpperHandle;
    inplaceMoveContourAlongNormal(
        refineBodyUpperHandle,
        _body,
        ContourMoveParameter::LHS,
        _toolRadius
    );
    inplaceMoveContourAlongNormal(
        refineBodyUpperHandle,
        _handle,
        ContourMoveParameter::RHS,
        _toolRadius
    );

    auto refineBodyLowerHandle = bodyLowerHandle;
    inplaceMoveContourAlongNormal(
        refineBodyLowerHandle,
        _body,
        ContourMoveParameter::LHS,
        _toolRadius
    );
    inplaceMoveContourAlongNormal(
        refineBodyLowerHandle,
        _handle,
        ContourMoveParameter::RHS,
        _toolRadius
    );

    _refinementCurves.clear();
    _refinementCurves.push_back(extractPositions(refineBodyDrill));
    _refinementCurves.push_back(extractPositions(refineBodyUpperHandle));
    _refinementCurves.push_back(extractPositions(refineBodyLowerHandle));

    makeRenderable(refineBodyDrill);
    makeRenderable(refineBodyUpperHandle);
    makeRenderable(refineBodyLowerHandle);

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

    auto baseHandleInner = fixParametricContour(extractLhs(handleInner));
    auto baseBodyBack = fixParametricContour(extractLhs(bodyBack));
    prepareBaseTrimmedArea(baseHandleInner, baseBodyBack);

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

std::vector<glm::dvec2>
        SienkiewiczkModelIntersectionsHandler::joinIntersections(
    const std::vector<glm::dvec2> &lhs,
    const std::vector<glm::dvec2> &rhs
)
{
    std::vector<glm::dvec2> output(lhs);
    auto firstDistance = glm::length(lhs.front() - rhs.front());
    auto lastDistance = glm::length(lhs.front()- rhs.back());

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
    // hack: assume that zero index is all we want
    std::cout << "prepping body trimmed area" << std::endl;

    ContourMerger merger;

    fw::CurveSimplifier<glm::dvec2, double> curveSimplifier;
    auto simplifiedUpperHole = curveSimplifier.simplify(
        removeJoinedVertices(handleUpperHole[0])
    );
    auto simplifiedLowerHole = curveSimplifier.simplify(
        removeJoinedVertices(handleLowerHole[0])
    );
    auto simplifiedDrillHole = curveSimplifier.simplify(
        removeJoinedVertices(drillHole[0])
    );

    auto fixedBackContour = backContour[0];
    std::reverse(std::begin(fixedBackContour), std::end(fixedBackContour));

    auto fixedFrontContour = frontContour[0];

    auto joined = joinSegmentWithLooped(fixedBackContour, simplifiedUpperHole);
    joined = joinSegmentWithLooped(joined, simplifiedLowerHole);

    auto joinedFront = joinSegmentWithLooped(
        fixedFrontContour,
        simplifiedDrillHole,
        1
    );

    _bodyParametricContours.push_back(joinIntersections(joined, joinedFront));

    std::cout << "finished prepping body trimmed area" << std::endl;
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

void SienkiewiczkModelIntersectionsHandler::prepareBaseTrimmedArea(
    const std::vector<std::vector<glm::dvec2>>& handleInnerContours,
    const std::vector<std::vector<glm::dvec2>>& bodyBackContour
)
{
    std::cout << "prepping base trimmed area" << std::endl;
    auto handleInner = handleInnerContours[0];
    auto handleInnerRev = handleInner;
    std::reverse(std::begin(handleInnerRev), std::end(handleInnerRev));

    auto bodyBack = bodyBackContour[0];
    auto bodyBackRev = bodyBack;
    std::reverse(std::begin(bodyBackRev), std::end(bodyBackRev));

    auto cutBase = cutCurveUsingCurves(
        handleInner,
        bodyBackRev,
        bodyBack
    );

    auto cutBody = cutCurveUsingCurves(
        bodyBack,
        handleInnerRev,
        handleInner
    );

    std::vector<glm::dvec2> unifiedContour(cutBase);

    std::copy(
        std::begin(cutBody),
        std::end(cutBody),
        std::back_inserter(unifiedContour)
    );

    std::reverse(std::begin(unifiedContour), std::end(unifiedContour));

    _baseParametricContours.clear();
    _baseParametricContours.push_back(unifiedContour);
    std::cout << "finito" << std::endl;
}

std::vector<glm::dvec2>
        SienkiewiczkModelIntersectionsHandler::cutCurveUsingCurves(
    const std::vector<glm::dvec2>& input,
    const std::vector<glm::dvec2>& lhs,
    const std::vector<glm::dvec2>& rhs
)
{
    auto lhsInters = findAnyCurveIntersection(input, lhs);
    auto rhsInters = findAnyCurveIntersection(input, rhs);

    auto firstInt = lhsInters.first;
    auto secondInt = rhsInters.first;

    if (firstInt == -1 && secondInt == -1) { return input; }

    if (secondInt == -1) secondInt = input.size() - 1;
    if (firstInt > secondInt)
    {
        std::swap(firstInt, secondInt);
    }

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

std::vector<glm::dvec2>
        SienkiewiczkModelIntersectionsHandler::removeJoinedVertices(
    const std::vector<glm::dvec2>& contour
)
{
    std::vector<glm::dvec2> output;
    const double eps = 10e-6;
    for (const auto& position: contour)
    {
        if (output.size() == 0 || glm::length(output.back() - position) > eps)
        {
            output.push_back(position);
        }
    }
    return output;
}

std::vector<glm::dvec2>
        SienkiewiczkModelIntersectionsHandler::joinSegmentWithLooped(
    const std::vector<glm::dvec2>& lhs,
    const std::vector<glm::dvec2>& rhs,
    int direction
)
{
    std::vector<std::pair<int, int>> intersectingStarts{};

    for (auto i = 0; i + 1 < lhs.size(); ++i)
    {
        for (auto j = 0; j + 1 < rhs.size(); ++j)
        {
            auto intersection = fw::intersectSegments<glm::dvec2, double>(
                lhs[i],
                lhs[i+1],
                rhs[j],
                rhs[j+1]
            );

            if (intersection.kind == fw::GeometricIntersectionKind::Single)
            {
                std::cout << "inter: " << i << " " << j << std::endl;
                intersectingStarts.push_back({i, j});
            }
        }
    }

    if (intersectingStarts.size() != 2)
    {
        std::cout << "unsupported case";
        return lhs;
    }

    std::vector<glm::dvec2> output;

    std::copy(
        std::begin(lhs),
        std::begin(lhs) + intersectingStarts[0].first,
        std::back_inserter(output)
    );

    copyContourInDirection(
        output,
        rhs,
        intersectingStarts[0].second,
        intersectingStarts[1].second,
        direction
    );

    std::copy(
        std::begin(lhs) + intersectingStarts[1].first + 1,
        std::end(lhs),
        std::back_inserter(output)
    );

    return output;
}

void SienkiewiczkModelIntersectionsHandler::copyContourInDirection(
    std::vector<glm::dvec2> &output,
    const std::vector<glm::dvec2> contour,
    int from,
    int to,
    int direction
)
{
    int current = from;
    while (true)
    {
        bool willStop = current == to;
        output.push_back(contour[current]);
        current = (current + direction) % static_cast<int>(contour.size());
        if (current < 0) current += contour.size();
        if (willStop) { break; }
    }
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

    return aStart + (aEnd - aStart) * intersection.t0;
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

void SienkiewiczkModelIntersectionsHandler
        ::inplaceMoveContourAlongNormal(
    std::vector<fw::ParametricSurfaceIntersection>& intersection,
    std::shared_ptr<fw::IParametricSurfaceUV> surface,
    ContourMoveParameter moveParameter,
    double distance
)
{
    for (auto &param: intersection)
    {
        glm::dvec2 parameter = moveParameter == ContourMoveParameter::LHS
            ? param.lhsParameters
            : param.rhsParameters;

        auto normal = surface->getNormal(parameter);
        normal = glm::normalize(normal);

        param.scenePosition = param.scenePosition + distance * normal;
    }
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

std::vector<glm::dvec3> SienkiewiczkModelIntersectionsHandler::extractPositions(
    const std::vector<fw::ParametricSurfaceIntersection>& intersection
)
{
    std::vector<glm::dvec3> output;
    for (auto& el: intersection)
    {
        output.push_back(el.scenePosition);
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

std::vector<std::vector<glm::dvec2>>
        SienkiewiczkModelIntersectionsHandler::getBaseParametricContours()
{
    return _baseParametricContours;
}

std::vector<std::vector<glm::dvec3>>
        SienkiewiczkModelIntersectionsHandler::getRefinementCurves()
{
    return _refinementCurves;
}

}
