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

std::vector<glm::dvec2> ContourMerger::merge2D(
    const std::vector<glm::dvec2> &lhs,
    const std::vector<glm::dvec2> &rhs
) const
{
    std::vector<glm::dvec3> lhs3d, rhs3d;

    std::transform(
        std::begin(lhs),
        std::end(lhs),
        std::back_inserter(lhs3d),
        [](const glm::dvec2& input)
        {
            return glm::dvec3{input.x, 0.0, input.y};
        }
    );

    std::transform(
        std::begin(rhs),
        std::end(rhs),
        std::back_inserter(rhs3d),
        [](const glm::dvec2& input)
        {
            return glm::dvec3{input.x, 0.0, input.y};
        }
    );

    auto merged3d = merge2D(lhs3d, rhs3d);
    std::vector<glm::dvec2> output;

    std::transform(
        std::begin(merged3d),
        std::end(merged3d),
        std::back_inserter(output),
        [](const glm::dvec3& input)
        {
            return glm::dvec2{input.x, input.z};
        }
    );

    return output;
}

std::vector<glm::dvec3> ContourMerger::merge2D(
    const std::vector<glm::dvec3> &lhs,
    const std::vector<glm::dvec3> &rhs
) const
{
    fw::CurveSimplifier<glm::dvec3, double> curveSimplifier;
    curveSimplifier.setMinimumMergeCosine(0.99999);
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

            if (intersection.kind != fw::GeometricIntersectionKind::None)
            {
                std::cout << i << " with " << j << std::endl;
                auto intPt = lhs[i] + intersection.t0 * (lhs[i+1] - lhs[i]);
                intersectionMarkers.push_back({i, j, intPt});
            }
        }
    }

    // todo: fix to general algorithm
    if (intersectionMarkers.size() == 0)
    {
        // no intersection
        std::cout << "Contour: cannot merge because there is no intersection."
            << std::endl;
        return lhs;
    }

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
