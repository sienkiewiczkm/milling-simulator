#include "ZigZagPathGenerator.hpp"
#include <iostream>

namespace ms
{

ZigZagPathGenerator::ZigZagPathGenerator()
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
            glm::dvec2{+_blockSize.x / 2 + offsetFactor * _toolRadius, z}
        });
    }
}

void ZigZagPathGenerator::subdivideZigZags()
{
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
                {from.x, _baseHeight, from.y}
            });

            movements.push_back({
                PathMovementType::Milling,
                {to.x, _baseHeight, to.y}
            });
        }
        leftToRight = !leftToRight;
    }

    return movements;
}

}
