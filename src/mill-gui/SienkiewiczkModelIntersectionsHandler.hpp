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

    void setScaledToolRadius(double toolRadius, double refineToolRadius);
    void findIntersections();
    std::vector<glm::dvec3> getObjectContour(glm::dmat4 objectTransformation);
    std::vector<glm::dvec3> getHandleHoleContour(
        glm::dmat4 objectTransformation
    );
    void render() const;

    std::vector<std::vector<glm::dvec2>> getDrillParametricContours();
    std::vector<std::vector<glm::dvec2>> getHandleParametricContours();
    std::vector<std::vector<glm::dvec2>> getBodyParametricContours();
    std::vector<std::vector<glm::dvec2>> getBaseParametricContours();

    std::vector<std::vector<glm::dvec3>> getRefinementCurves();

protected:
    /// \brief Forces join of two intersections. It connects closest end-points
    //         of the intersection together making a circle.
    std::vector<fw::ParametricSurfaceIntersection> joinIntersections(
        const std::vector<fw::ParametricSurfaceIntersection>& lhs,
        const std::vector<fw::ParametricSurfaceIntersection>& rhs
    );

    std::vector<glm::dvec2> joinIntersections(
        const std::vector<glm::dvec2>& lhs,
        const std::vector<glm::dvec2>& rhs
    );

    enum class ContourMoveParameter
    {
        LHS,
        RHS,
    };

    void prepareBodyTrimmedArea(
        std::vector<std::vector<glm::dvec2>> backContour,
        std::vector<std::vector<glm::dvec2>> frontContour,
        std::vector<std::vector<glm::dvec2>> drillHole,
        std::vector<std::vector<glm::dvec2>> handleUpperHole,
        std::vector<std::vector<glm::dvec2>> handleLowerHole
    );

    void prepareDrillTrimmedArea(
        const std::vector<std::vector<glm::dvec2>>&
            visibleParametrizationContour,
        const std::vector<std::vector<glm::dvec2>>& bodyIntersectionContour
    );

    void prepareHandleTrimmedArea(
        const std::vector<std::vector<glm::dvec2>>& handleOuterContours,
        const std::vector<std::vector<glm::dvec2>>& handleInnerContours,
        const std::vector<std::vector<glm::dvec2>>& handleUpperConnection,
        const std::vector<std::vector<glm::dvec2>>& handleLowerConnection
    );

    void prepareBaseTrimmedArea(
        const std::vector<std::vector<glm::dvec2>>& handleInnerContours,
        const std::vector<std::vector<glm::dvec2>>& bodyBackContour
    );

    std::vector<glm::dvec3> moveContourAlongFlattenedNormal(
        const std::vector<fw::ParametricSurfaceIntersection>& intersection,
        std::shared_ptr<fw::IParametricSurfaceUV> surface,
        ContourMoveParameter moveParameter,
        double distance
    );

    void inplaceMoveContourAlongNormal(
        std::vector<fw::ParametricSurfaceIntersection>& intersection,
        std::shared_ptr<fw::IParametricSurfaceUV> surface,
        ContourMoveParameter moveParameter,
        double distance
    );

    std::vector<glm::dvec2> cutCurveUsingCurves(
        const std::vector<glm::dvec2>& input,
        const std::vector<glm::dvec2>& lhs,
        const std::vector<glm::dvec2>& rhs
    );

    std::vector<glm::dvec2> joinSegmentWithLooped(
        const std::vector<glm::dvec2>& lhs,
        const std::vector<glm::dvec2>& rhs,
        int direction = -1
    );

    std::vector<glm::dvec2> removeJoinedVertices(
        const std::vector<glm::dvec2>& contour
    );

    void copyContourInDirection(
        std::vector<glm::dvec2> &output,
        const std::vector<glm::dvec2> contour,
        int from,
        int to,
        int direction
    );

    glm::dvec2 getIntersectionPoint(
        const glm::dvec2& aStart,
        const glm::dvec2& aEnd,
        const glm::dvec2& bStart,
        const glm::dvec2& bEnd
    );

    std::pair<int, int> findAnyCurveIntersection(
        const std::vector<glm::dvec2>& lhs,
        const std::vector<glm::dvec2>& rhs
    );

    std::pair<int, int> findCurveAutoIntersection(
        const std::vector<glm::dvec2>& lhs
    );

    void makeRenderable(
        const std::vector<fw::ParametricSurfaceIntersection>& intersection
    );

    void makeRenderable(
        const std::vector<glm::dvec3>& contour
    );

    std::vector<std::vector<glm::dvec2>> fixParametricContour(
        const std::vector<glm::dvec2> &contour
    );

    std::vector<glm::dvec2> extractRhs(
        const std::vector<fw::ParametricSurfaceIntersection>& intersection
    );

    std::vector<glm::dvec2> extractLhs(
        const std::vector<fw::ParametricSurfaceIntersection>& intersection
    );

    std::vector<glm::dvec3> extractPositions(
        const std::vector<fw::ParametricSurfaceIntersection>& intersection
    );

private:
    std::vector<glm::dvec3> _objectShiftedContour;
    std::vector<glm::dvec3> _holeShiftedContour;

    fw::ParametricSurfaceIntersectionFinder _intersectionFinder;
    std::shared_ptr<fw::IParametricSurfaceUV> _body;
    std::shared_ptr<fw::IParametricSurfaceUV> _handle;
    std::shared_ptr<fw::IParametricSurfaceUV> _drill;
    std::shared_ptr<fw::IParametricSurfaceUV> _base;
    std::vector<std::shared_ptr<fw::PolygonalLine>>
        _intersectionsRepresentations;

    double _toolRadius;
    double _refineToolRadius;

    std::vector<std::vector<glm::dvec2>> _drillParametricContour;
    std::vector<std::vector<glm::dvec2>> _handleParametricContour;
    std::vector<std::vector<glm::dvec2>> _bodyParametricContours;
    std::vector<std::vector<glm::dvec2>> _baseParametricContours;

    std::vector<std::vector<glm::dvec3>> _refinementCurves;
};

}
