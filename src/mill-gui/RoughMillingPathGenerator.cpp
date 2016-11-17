#include "RoughMillingPathGenerator.hpp"
#include "IParametricSurfaceUV.hpp"
#include "CurveSimplifier.hpp"

#include <algorithm>
#include <iostream>
#include <limits>

namespace ms
{

RoughMillingPathGenerator::RoughMillingPathGenerator():
    _samplingResolution{512, 512},
    _baseHeight{20.5},
    _safeHeight{50.5}
{
}

RoughMillingPathGenerator::~RoughMillingPathGenerator()
{
}

void RoughMillingPathGenerator::setBaseHeight(double baseHeight)
{
    _baseHeight = baseHeight;
}

void RoughMillingPathGenerator::setCuttingToolRadius(double radius)
{
    _toolRadius = radius;
}

void RoughMillingPathGenerator::setWorkingArea(
    glm::dvec3 blockSize,
    glm::dmat4 worldMatrix
)
{
    _blockSize = blockSize;
    _blockWorldMatrix = worldMatrix;
    _blockWorldMatrixInv = glm::inverse(worldMatrix);
}

void RoughMillingPathGenerator::setWorkingAreaResolution(
    glm::ivec2 areaResolution
)
{
    _workingAreaResolution = areaResolution;
}

void RoughMillingPathGenerator::setSamplingResolution(
    glm::ivec2 samplingResolution
)
{
    _samplingResolution = samplingResolution;
}

void RoughMillingPathGenerator::addParametricSurface(
    std::shared_ptr<fw::IParametricSurfaceUV> surface,
    glm::dmat4 worldMatrix
)
{
    _surfaces.push_back(SurfaceTransformationTuple(surface, worldMatrix));
}

void RoughMillingPathGenerator::bake()
{
    _heightmap.resize(_workingAreaResolution.x * _workingAreaResolution.y);
    std::fill(std::begin(_heightmap), std::end(_heightmap), _baseHeight);

    for (const auto &surface: _surfaces)
    {
        bakeSurfaceIntoHeightmap(surface);
    }

    bakeZigZagPath();
    simplifyPath();
}

std::vector<PathMovement> RoughMillingPathGenerator::buildPaths()
{
    std::vector<PathMovement> realPaths;
    for (auto &position: _path)
    {
        PathMovement movement;
        movement.type = PathMovementType::Milling;
        movement.position = position;
        realPaths.push_back(movement);
    }
    for (int i = 0; i < 10; ++i)
    {
        std::cout << "(" << _path[i].x << " ; " << _path[i].y << " ; " << _path[i].z << ")" << std::endl;
    }
    return realPaths;
}

void RoughMillingPathGenerator::bakeSurfaceIntoHeightmap(
    const SurfaceTransformationTuple &surface
)
{
    for (auto x = 0; x < _samplingResolution.x; ++x)
    {
        auto u = x / static_cast<double>(_samplingResolution.x - 1);

        auto curve = std::get<0>(surface)->getConstParameterCurve(
            fw::ParametrizationAxis::U,
            u
        );

        for (auto y = 0; y < _samplingResolution.y; ++y)
        {
            auto v = y / static_cast<double>(_samplingResolution.y - 1);

            auto pointLocalPosition = curve->evaluate(v);

            auto pointWorldPosition = std::get<1>(surface)
                * glm::dvec4(pointLocalPosition, 1.0);

            // todo: count inverse in (may be relevant in the future)
            auto pointBlockFrame = pointWorldPosition;

            glm::dvec2 heightmapCoord {
                pointBlockFrame.x / _blockSize.x + 0.5,
                pointBlockFrame.z / _blockSize.z + 0.5
            };

            if (heightmapCoord.x < 0.0 || heightmapCoord.x > 1.0
                || heightmapCoord.y < 0.0 || heightmapCoord.y > 1.0)
            {
                continue;
            }

            glm::ivec2 memoryCoord {
                static_cast<int>(heightmapCoord.x * _workingAreaResolution.x),
                static_cast<int>(heightmapCoord.y * _workingAreaResolution.y)
            };

            auto index
                = _workingAreaResolution.x * memoryCoord.y + memoryCoord.x;

            _heightmap[index] = std::max(
                _heightmap[index],
                pointBlockFrame.y
            );
        }
    }
}

void RoughMillingPathGenerator::bakeZigZagPath()
{
    _path.clear();

    auto exactToolRadiusCoverage =
        _toolRadius
        * glm::dvec2{_workingAreaResolution.x, _workingAreaResolution.y}
        / glm::dvec2{_blockSize.x, _blockSize.z};

    std::cout << "Exact tool radius coverage: "
        << "x= " << exactToolRadiusCoverage.x
        << "y= " << exactToolRadiusCoverage.y
        << std::endl;

    auto heightCheckRadiusCoverage = glm::ivec2{
        std::ceil(exactToolRadiusCoverage.x),
        std::ceil(exactToolRadiusCoverage.y)
    };

    auto followAbilityRadiusCoverage = glm::ivec2(
        std::floor(exactToolRadiusCoverage.x),
        std::floor(exactToolRadiusCoverage.y)
    );

    glm::ivec2 initialCellPosition{
        static_cast<int>(followAbilityRadiusCoverage.x/2),
        static_cast<int>(followAbilityRadiusCoverage.y/2)
    };

    int xDirection = 1;

    auto cellPosition = initialCellPosition;
    while (cellPosition.x >= -followAbilityRadiusCoverage.x
        && cellPosition.y >= -followAbilityRadiusCoverage.y
        && cellPosition.x < _workingAreaResolution.x
            + followAbilityRadiusCoverage.x
        && cellPosition.y < _workingAreaResolution.y
            + followAbilityRadiusCoverage.y)
    {
        auto cellCenter = getCellWorldCenter(cellPosition);
        auto height = getHighestValueInArea(
            cellPosition - followAbilityRadiusCoverage,
            cellPosition + followAbilityRadiusCoverage
        );

        _path.push_back({cellCenter.x, height, cellCenter.y});

        auto xForwardReach = cellPosition.x
            + xDirection * followAbilityRadiusCoverage.x;
        if (xForwardReach <= -followAbilityRadiusCoverage.x/2
            || xForwardReach >=
                _workingAreaResolution.x + followAbilityRadiusCoverage.y/2)
        {
            xDirection = -xDirection;
            cellPosition.y += followAbilityRadiusCoverage.y;
        }
        else
        {
            cellPosition.x += xDirection;
        }
    }
}

void RoughMillingPathGenerator::simplifyPath()
{
    fw::CurveSimplifier<glm::dvec3, double> curveSimplifier;
    auto simplifiedPath = curveSimplifier.simplify(_path);

    std::cout << "Pre-simplified had " << _path.size() << "steps. "
        << "Simplified path has " << simplifiedPath.size() << "steps."
        << std::endl;

    // todo: fix, it's slow!
    glm::dvec3 safeEntryPoint{_path[0].x, _safeHeight, _path[0].z};
    simplifiedPath.insert(begin(simplifiedPath), safeEntryPoint);

    _path = simplifiedPath;
}

glm::dvec2 RoughMillingPathGenerator::getCellWorldCenter(glm::ivec2 coordinate)
{
    auto x = (coordinate.x + 0.5)
        / static_cast<double>(_workingAreaResolution.x);
    auto y = (coordinate.y + 0.5)
        / static_cast<double>(_workingAreaResolution.y);

    return {
        (x - 0.5) * _blockSize.x,
        (y - 0.5) * _blockSize.z
    };
}

double RoughMillingPathGenerator::getHighestValueInArea(
    glm::ivec2 minCoordinate,
    glm::ivec2 maxCoordinate
) const
{
    minCoordinate = glm::max(minCoordinate, glm::ivec2{0, 0});
    maxCoordinate = glm::min(
        maxCoordinate,
        _workingAreaResolution - glm::ivec2{1, 1}
    );

    auto maxValue = std::numeric_limits<double>::min();
    for (auto y = minCoordinate.y; y < maxCoordinate.y; ++y)
    {
        for (auto x = minCoordinate.x; x < maxCoordinate.x; ++x)
        {
            maxValue = std::max(
                maxValue,
                _heightmap[_workingAreaResolution.x * y + x]
            );
        }
    }

    return maxValue + 1.5; // add 1.5mm just to be safe
}

}
