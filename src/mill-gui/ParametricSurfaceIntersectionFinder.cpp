#include "ParametricSurfaceIntersectionFinder.hpp"
#include "ParametricSurfaceClosestPointNaiveFinder.hpp"
#include <iostream>

namespace fw
{

ParametricSurfaceIntersectionFinder::ParametricSurfaceIntersectionFinder()
{
}

ParametricSurfaceIntersectionFinder::~ParametricSurfaceIntersectionFinder()
{
}

std::vector<ParametricSurfaceIntersection>
        ParametricSurfaceIntersectionFinder::intersect(
    const std::shared_ptr<IParametricSurfaceUV> lhs,
    const std::shared_ptr<IParametricSurfaceUV> rhs,
    glm::vec3 neighbourhoodPoint
)
{
    // todo: parametric surface transformations should be taken into the account
    _intersectionCurve = IntersectionCurve();
    _lhs = lhs;
    _rhs = rhs;

    ParametricSurfaceClosestPointNaiveFinder closestPointFinder;
    closestPointFinder.setReferencePoint(neighbourhoodPoint);
    closestPointFinder.setSamplingResolution({512, 512});
    auto lhsClosest = closestPointFinder.find(*lhs);
    auto rhsClosest = closestPointFinder.find(*rhs);

    _newtonIterable.setCovergenceThreshold(0.00001);
    _newtonIterable.setPlaneDistance(0.04);
    _newtonIterable.setSurfaces(lhs, rhs);

    _intersectionCurve.setLooping(true);
    _intersectionCurve.setLoopbackMinimumIndexDifference(20);
    _intersectionCurve.setLoopBackDistance(0.05);

    if (!iterateInDirection(1.0, lhsClosest, rhsClosest))
    {
        _intersectionCurve.reverse();
        iterateInDirection(-1.0, lhsClosest, rhsClosest);
    }

    return _intersectionCurve.getCurvePoints();
}

bool ParametricSurfaceIntersectionFinder::iterateInDirection(
    double tangentMultipler,
    glm::dvec2 initialLhsParams,
    glm::dvec2 initialRhsParams
)
{
    auto currentLhsParams = initialLhsParams;
    auto currentRhsParams = initialRhsParams;

    for (int i = 0; i < 500; ++i)
    {
        auto lhsNormal = _lhs->getNormal(currentLhsParams);
        auto rhsNormal = _rhs->getNormal(currentRhsParams);
        auto currentTangent = glm::cross(lhsNormal, rhsNormal);

        _newtonIterable.setTangentVector(tangentMultipler * currentTangent);

        // todo: check if intersection is valid
        auto intersectionResult = _newtonIterator.iterate(
            _newtonIterable,
            glm::dvec4{currentLhsParams, currentRhsParams}
        );

        if (intersectionResult.exitStatus != NewtonIterationExitStatus::Success)
        {
            std::string stopReasonText;
            switch(intersectionResult.exitStatus)
            {
            case NewtonIterationExitStatus::IterationLimitReached:
                stopReasonText = "Iteration limit reached.";
                break;
            case NewtonIterationExitStatus::InvalidParameterReached:
                stopReasonText = "Invalid parametrization reached.";
                break;
            default:
                break;
            }
            std::cout << "Stopping, reason: " << stopReasonText << std::endl;

            if (intersectionResult.exitStatus
                    == NewtonIterationExitStatus::InvalidParameterReached)
            {
                break;
            }
        }

        auto intersectionParameters = intersectionResult.parameters;
        auto intersectionPoint = _lhs->getPosition(
            {intersectionParameters.x, intersectionParameters.y}
        );

        currentLhsParams = {intersectionParameters.x, intersectionParameters.y};
        currentRhsParams = {intersectionParameters.z, intersectionParameters.w};

        auto addingResult = _intersectionCurve.addCurvePoint({
            currentLhsParams,
            currentRhsParams,
            intersectionPoint
        });

        if (addingResult == IntersectionCurveAddingResult::LoopedBack)
        {
            return true;
        }
    }

    return false;
}

}

