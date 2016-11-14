#include "ZigZagPathGenerator.hpp"
#include "GeometricIntersections.hpp"
#include <iostream>

namespace ms
{

ZigZagPathGenerator::ZigZagPathGenerator():
    _safeHeight{51.5}
{
}

ZigZagPathGenerator::~ZigZagPathGenerator()
{
}


void ZigZagPathGenerator::setRejectionMode(ZigZagRejectionMode rejectionMode)
{
    _rejectionMode = rejectionMode;
}

void ZigZagPathGenerator::setBaseHeight(double baseHeight)
{
    _baseHeight = baseHeight;
}

void ZigZagPathGenerator::setCuttingToolRadius(double radius)
{
    _toolRadius = radius;
}

void ZigZagPathGenerator::setWorkingArea(
    glm::dvec3 blockSize,
    glm::dmat4 worldMatrix
)
{
    _blockSize = blockSize;
    _blockWorldMatrix = worldMatrix;
    _blockWorldMatrixInv = glm::inverse(worldMatrix);
}

void ZigZagPathGenerator::setInbetweenDistance(double distance)
{
    _inbetweenDistance = distance;
}

void ZigZagPathGenerator::setObjectContours(
    const std::vector<glm::dvec3>& objectContour
)
{
    _objectContour = objectContour;
}

void ZigZagPathGenerator::bake()
{
    _segments.clear();
    createUncutZigZags();
    subdivideZigZags();
}

void ZigZagPathGenerator::createUncutZigZags()
{
    auto offsetFactor = 1.3;
    for (auto z = -_blockSize.z / 2 - offsetFactor * _toolRadius;
        z < _blockSize.z / 2 + offsetFactor * _toolRadius;
        z += _inbetweenDistance)
    {
        _segments.push_back({});
        _segments.back().push_back({
            glm::dvec2{-_blockSize.x / 2 - offsetFactor * _toolRadius, z},
            glm::dvec2{+_blockSize.x / 2 + offsetFactor * _toolRadius, z},
            -1,
            -1
        });
    }
}

void ZigZagPathGenerator::subdivideZigZags()
{
    for (auto line = 0; line < _segments.size(); ++line)
    {
        const auto& segmentLine = _segments[line];
        std::vector<SegmentTuple> subdividedLine;
        for (const auto &segment: segmentLine)
        {
            auto from = std::get<0>(segment);
            auto to = std::get<1>(segment);

            std::vector<std::tuple<double, int>> intersections;
            for (auto i = 0; i < _objectContour.size() - 1; ++i)
            {

                auto intersection = fw::intersectSegments<glm::dvec2, double>(
                    from,
                    to,
                    {_objectContour[i].x, _objectContour[i].z},
                    {_objectContour[i+1].x, _objectContour[i+1].z}
                );

                if (intersection.kind == fw::GeometricIntersectionKind::None)
                {
                    continue;
                }

                auto intersectionPoint = from
                    + (to-from) * intersection.singleIntersectionParameter;

                intersections.push_back({intersectionPoint.x, i});
            }

            intersections.push_back({from.x, -1});
            intersections.push_back({to.x, -1});

            std::sort(std::begin(intersections), std::end(intersections));

            for (auto i = 0; i < intersections.size(); i += 2)
            {
                subdividedLine.push_back({
                    glm::dvec2{std::get<0>(intersections[i]), from.y},
                    glm::dvec2{std::get<0>(intersections[i+1]), from.y},
                    std::get<1>(intersections[i]),
                    std::get<1>(intersections[i+1])
                });
            }
        }

        _segments[line] = subdividedLine;
    }
}

std::vector<PathMovement> ZigZagPathGenerator::buildPaths()
{
    std::vector<PathMovement> movements;

    bool leftToRight = true;

    for (const auto& segmentLine: _segments)
    {
        for (const auto& segment: segmentLine)
        {
            auto from = std::get<0>(segment);
            auto to = std::get<1>(segment);
            if (!leftToRight) { std::swap(from, to); }

            movements.push_back({
                PathMovementType::Milling,
                {from.x, _safeHeight, from.y}
            });

            movements.push_back({
                PathMovementType::Milling,
                {from.x, _baseHeight, from.y}
            });

            movements.push_back({
                PathMovementType::Milling,
                {to.x, _baseHeight, to.y}
            });

            movements.push_back({
                PathMovementType::Milling,
                {to.x, _safeHeight, to.y}
            });
        }

        //leftToRight = !leftToRight;
    }

    return movements;
}

}
