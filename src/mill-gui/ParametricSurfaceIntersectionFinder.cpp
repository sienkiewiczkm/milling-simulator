#include "ParametricSurfaceIntersectionFinder.hpp"
#include "ParametricSurfaceClosestPointNaiveFinder.hpp"
#include "SurfaceIntersectionNewtonIterable.hpp"
#include <iostream>

namespace fw
{

ParametricSurfaceIntersection::ParametricSurfaceIntersection():
    lhsParameters{},
    rhsParameters{},
    scenePosition{}
{
}

ParametricSurfaceIntersection::ParametricSurfaceIntersection(
    glm::dvec2 lhsParameters,
    glm::dvec2 rhsParameters,
    glm::dvec3 scenePosition
):
    lhsParameters{lhsParameters},
    rhsParameters{rhsParameters},
    scenePosition{scenePosition}
{
}

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

    std::vector<ParametricSurfaceIntersection> intersectionCurve{};
    //intersectionCurve.push_back(neighbourhoodPoint);
    //intersectionCurve.push_back(neighbourhoodPoint + glm::vec3(0,1,0));

    ParametricSurfaceClosestPointNaiveFinder closestPointFinder;
    closestPointFinder.setReferencePoint(neighbourhoodPoint);
    closestPointFinder.setSamplingResolution({512, 512});
    auto lhsClosest = closestPointFinder.find(*lhs);
    auto rhsClosest = closestPointFinder.find(*rhs);

    SurfaceIntersectionNewtonIterator newtonIterator;
    SurfaceIntersectionNewtonIterable newtonIterable;
    newtonIterable.setCovergenceThreshold(0.00001);
    newtonIterable.setPlaneDistance(0.1);
    newtonIterable.setSurfaces(lhs, rhs);

    auto currentLhsParams = lhsClosest;
    auto currentRhsParams = rhsClosest;

    for (int i = 0; i < 100; ++i)
    {
        auto lhsNormal = lhs->getNormal(currentLhsParams);
        auto rhsNormal = rhs->getNormal(currentRhsParams);
        auto currentTangent = glm::cross(lhsNormal, rhsNormal);

        newtonIterable.setTangentVector(currentTangent);

        // todo: check if intersection is valid
        auto intersectionResult = newtonIterator.iterate(
            newtonIterable,
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

            if (intersectionResult.exitStatus == NewtonIterationExitStatus::InvalidParameterReached)
            {
                break;
            }
        }

        auto intersectionParameters = intersectionResult.parameters;
        auto intersectionPoint = lhs->getPosition(
            {intersectionParameters.x, intersectionParameters.y}
        );

        currentLhsParams = {intersectionParameters.x, intersectionParameters.y};
        currentRhsParams = {intersectionParameters.z, intersectionParameters.w};

        intersectionCurve.push_back({
            currentLhsParams,
            currentRhsParams,
            intersectionPoint
        });
    }

    return intersectionCurve;
}

}

