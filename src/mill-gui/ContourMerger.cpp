#include "ContourMerger.hpp"
#include "PointQuadtree.hpp"
#include "GeometricIntersections.hpp"
#include "CurveSimplifier.hpp"

#include <iostream>

namespace ms
{


ContourMerger::ContourMerger()
{
}

ContourMerger::~ContourMerger()
{
}

std::vector<glm::dvec3> ContourMerger::merge2D(
    const std::vector<glm::dvec3> &lhs,
    const std::vector<glm::dvec3> &rhs
) const
{
    fw::CurveSimplifier<glm::dvec3, double> curveSimplifier;
    curveSimplifier.setMinimumMergeCosine(0.9999);
    auto lhsSimplified = curveSimplifier.simplify(lhs);
    auto rhsSimplified = curveSimplifier.simplify(rhs);

    std::vector<glm::dvec3> mergedContour;

    // todo: ensure direction

    std::vector<std::tuple<int, int, glm::dvec3>> intersectionMarkers;

    for (auto i = 0; i < lhs.size() - 1; ++i)
    {
        for (auto j = 0; j < rhs.size() - 1; ++j)
        {
            auto intersection = fw::intersectSegments<glm::dvec3, double>(
                lhs[i],
                lhs[i+1],
                rhs[j],
                rhs[j+1]
            );

            if (intersection.kind == fw::GeometricIntersectionKind::Single)
            {
                std::cout << i << " with " << j << std::endl;
                auto intPt = lhs[i]
                    + intersection.singleIntersectionParameter *
                        (lhs[i+1] - lhs[i]);
                intersectionMarkers.push_back({i, j, intPt});
            }
        }
    }

    // todo: fix to general algorithm

    followContourAdding(
        lhs,
        mergedContour,
        0,
        std::get<0>(intersectionMarkers[0])
    );

    // add intersection
    mergedContour.push_back(std::get<2>(intersectionMarkers[0]));

    followContourAdding(
        rhs,
        mergedContour,
        std::get<1>(intersectionMarkers[0]) + 1,
        std::get<1>(intersectionMarkers[1])
    );

    // add intersection
    mergedContour.push_back(std::get<2>(intersectionMarkers[1]));

    followContourAdding(
        lhs,
        mergedContour,
        std::get<0>(intersectionMarkers[1]) + 1,
        lhs.size() - 1
    );

    return mergedContour;
}

void ContourMerger::followContourAdding(
    const std::vector<glm::dvec3> &lhs,
    std::vector<glm::dvec3>& output,
    int from,
    int to,
    int increase
) const
{
    auto current = from;
    output.push_back(lhs[current]);
    while (true)
    {
        current = (current + increase) % lhs.size();
        output.push_back(lhs[current]);
        bool willStop = current == to;
        if (willStop) { break; }
    }
}

}
