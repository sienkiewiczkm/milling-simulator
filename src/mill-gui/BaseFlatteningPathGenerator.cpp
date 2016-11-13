#include "BaseFlatteningPathGenerator.hpp"
#include "Common.hpp"

namespace ms
{

BaseFlatteningPathGenerator::BaseFlatteningPathGenerator():
    _baseHeight{20.5},
    _safeHeight{50.5}
{
}

BaseFlatteningPathGenerator::~BaseFlatteningPathGenerator()
{
}

void BaseFlatteningPathGenerator::setBaseHeight(double baseHeight)
{
    _baseHeight = baseHeight;
}

void BaseFlatteningPathGenerator::setCuttingToolRadius(double radius)
{
    _toolRadius = radius;
}

void BaseFlatteningPathGenerator::setWorkingArea(
    glm::dvec3 blockSize,
    glm::dmat4 worldMatrix
)
{
    _blockSize = blockSize;
    _blockWorldMatrix = worldMatrix;
    _blockWorldMatrixInv = glm::inverse(worldMatrix);
}

void BaseFlatteningPathGenerator::setObjectContours(
    const std::vector<glm::dvec3>& objectContour
)
{
    _objectContour = objectContour;
}

void BaseFlatteningPathGenerator::bake()
{
    _paths.clear();
    createBorderEqualizationPath();
}

std::vector<PathMovement> BaseFlatteningPathGenerator::buildPaths()
{
    std::vector<PathMovement> assembledPath;

    for (const auto &path: _paths)
    {
        auto ep = findSafeEntryPoint(path[0]);

        assembledPath.push_back({
            PathMovementType::Milling,
            {ep.x, _safeHeight, ep.z}
        });

        assembledPath.push_back({
            PathMovementType::Milling,
            ep
        });

        for (const auto &position: path)
        {
            assembledPath.push_back({
                PathMovementType::Milling,
                position
            });
        }
    }

    return assembledPath;
}

void BaseFlatteningPathGenerator::createBorderEqualizationPath()
{
    _paths.push_back(_objectContour);
}

glm::dvec3 BaseFlatteningPathGenerator::findSafeEntryPoint(
    const glm::dvec3& pos
) const
{
    auto offset = 1.2 * _toolRadius;
    if (_blockSize.x/2 - std::abs(pos.x) < _blockSize.z/2 - std::abs(pos.z))
    {
        return
        {
            (_blockSize.x/2+offset)*common::sgn(pos.x),
            pos.y,
            pos.z
        };
    }
    else
    {
        return
        {
            pos.x,
            pos.y,
            (_blockSize.z/2+offset)*common::sgn(pos.z)
        };
    }
}

}
