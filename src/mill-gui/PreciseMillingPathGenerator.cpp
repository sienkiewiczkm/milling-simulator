#include "PreciseMillingPathGenerator.hpp"
#include "GeometricIntersections.hpp"
#include "CurveSimplifier.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

namespace ms
{

PreciseMillingPathGenerator::PreciseMillingPathGenerator():
    _safeHeight{50.5},
    _baseHeight{20.5},
    _numScanLines{256},
    _maxScanLineResolution{256},
    _toolRadius{4.0},
    _blockSize{150.0, 50.0, 150.0},
    _workingAreaResolution{500, 500},
    _samplingResolution{512, 512}
{
}

PreciseMillingPathGenerator::~PreciseMillingPathGenerator()
{
}

void PreciseMillingPathGenerator::setNumScanLines(int scanLines)
{
    _numScanLines = scanLines;
}

void PreciseMillingPathGenerator::setNumLineMaximumResolution(
    int maxScanLineResolution
)
{
    _maxScanLineResolution = maxScanLineResolution;
}

void PreciseMillingPathGenerator::setParametricSurface(
    std::shared_ptr<fw::IParametricSurfaceUV> surface,
    const glm::dmat4 &surfaceTransformation
)
{
    _surface = surface;
    _surfaceTransformation = surfaceTransformation;
}

void PreciseMillingPathGenerator::clearCheckSurfaces()
{
    _checkSurface.clear();
    _checkSurfaceTransformation.clear();
}

void PreciseMillingPathGenerator::addCheckSurface(
    std::shared_ptr<fw::IParametricSurfaceUV> surface,
    const glm::dmat4& surfaceTransformation
)
{
    _checkSurface.push_back(surface);
    _checkSurfaceTransformation.push_back(surfaceTransformation);
}

void PreciseMillingPathGenerator::setBaseHeight(double baseHeight)
{
    _baseHeight = baseHeight;
}

void PreciseMillingPathGenerator::setCuttingToolRadius(double radius)
{
    _toolRadius = radius;
}

void PreciseMillingPathGenerator::setWorkingArea(
    glm::dvec3 blockSize,
    glm::dmat4 worldMatrix
)
{
    _blockSize = blockSize;
    _blockWorldMatrix = worldMatrix;
    _blockWorldMatrixInv = glm::inverse(worldMatrix);
}

void PreciseMillingPathGenerator::setWorkingAreaResolution(
    glm::ivec2 areaResolution
)
{
    _workingAreaResolution = areaResolution;
}

void PreciseMillingPathGenerator::setSamplingResolution(
    glm::ivec2 samplingResolution
)
{
    _samplingResolution = samplingResolution;
}

void PreciseMillingPathGenerator::setParametricSurfaceBoundaries(
    const std::vector<glm::dvec2>& boundaries
)
{
    _boundaries = boundaries;
}

void PreciseMillingPathGenerator::bake(
    bool inverseTrimmingSide,
    MillingDirection millingDirection
)
{
    _currentMillingDirection = millingDirection;

    bakeCheckSurfaceHeightmap();

    _rawPaths.clear();

    bool inversePath = false;
    for (auto row = 0; row < _numScanLines; ++row)
    {
        auto v = row / static_cast<double>(_numScanLines - 1);
        auto scanPoints = getScanLineIntersections(v, _boundaries);

        if (scanPoints.size()%2 == 1)
        {
            std::cout << "Whoopsie. There is odd numer of intersections. "
                << "Skipping." << std::endl;
            continue;
        }

        auto start = inverseTrimmingSide ? 0 : 1;

        for (auto i = start; i+1 < scanPoints.size(); i += 2)
        {
            std::vector<glm::dvec3> currentPath;

            std::cout << "total: " << scanPoints.size()
                << " generating from " << scanPoints[i] << " to "
                << scanPoints[i+1] << " ! " << std::endl;
            generatePathLine(currentPath, scanPoints[i], scanPoints[i+1], v);

            if (inversePath)
            {
                std::reverse(std::begin(currentPath), std::end(currentPath));
            }

            filterOutDamages(_rawPaths, currentPath);
        }

        if (scanPoints.size() > 1)
        {
            inversePath = !inversePath;
        }
    }

}

std::vector<PathMovement> PreciseMillingPathGenerator::buildPaths()
{
    std::vector<PathMovement> movements;

    for (const auto &path: _rawPaths)
    {
        if (path.size() < 2) { continue; }

        movements.push_back({
            PathMovementType::Milling,
            {path[0].x, _safeHeight, path[0].z}
        });

        for (const auto& position: path)
        {
            auto adjustedPosition = position;
            if (adjustedPosition.y < _baseHeight)
            {
                adjustedPosition.y = _baseHeight;
            }
            movements.push_back({PathMovementType::Milling, adjustedPosition});
        }

        movements.push_back({
            PathMovementType::Milling,
            {path.back().x, _safeHeight, path.back().z}
        });
    }

    return movements;
}

void PreciseMillingPathGenerator::bakeCheckSurfaceHeightmap()
{
    _toolHeightmapRadius = _toolRadius * (
        glm::dvec2{_workingAreaResolution}
            / glm::dvec2{_blockSize.x, _blockSize.z}
    );

    _checkHeightmap.resize(
        _workingAreaResolution.x * _workingAreaResolution.y
    );

    std::fill(
        std::begin(_checkHeightmap),
        std::end(_checkHeightmap),
        _baseHeight
    );


    for (auto surfId = 0; surfId < _checkSurface.size(); ++surfId)
    {
        auto surface = _checkSurface[surfId];
        auto transformation = _checkSurfaceTransformation[surfId];

        for (auto row = 0; row < _samplingResolution.y; ++row)
        {
            auto v = row / static_cast<double>(_samplingResolution.y - 1);

            auto checkCurve = surface->getConstParameterCurve(
                fw::ParametrizationAxis::V,
                v
            );

            for (auto i = 1; i < _samplingResolution.x; i += 2)
            {
                auto u = i / static_cast<double>(_samplingResolution.x - 1);
                auto pos = transformation
                    * glm::dvec4(checkCurve->evaluate(u), 1.0);
                bakePositionOnCheckHeightmap(glm::dvec3{pos});
            }
        }
    }
}

void PreciseMillingPathGenerator::bakePositionOnCheckHeightmap(
    const glm::dvec3& position
)
{
    if (position.y < _baseHeight) { return; }

    auto memoryCoord = getHeightmapCoord(position);

    if (memoryCoord.x < 0 || memoryCoord.x >= _workingAreaResolution.x
        || memoryCoord.y < 0 || memoryCoord.y >= _workingAreaResolution.y)
    {
        return;
    }

    auto index = _workingAreaResolution.x * memoryCoord.y + memoryCoord.x;

    _checkHeightmap[index] = std::max(
        _checkHeightmap[index],
        position.y
    );
}

bool PreciseMillingPathGenerator::doesPositionDamageCheckSurface(
    const glm::dvec3& toolPosition
)
{
    auto hcCoord = getHeightmapCoord(toolPosition);
    auto minHcCoord = hcCoord - glm::ivec2{glm::ceil(_toolHeightmapRadius)};
    auto maxHcCoord = hcCoord + glm::ivec2{glm::ceil(_toolHeightmapRadius)};

    double damageMagnitude = 0.0;
    bool objectDamaged = false;

    for (auto y = std::max(0, minHcCoord.y);
        y < std::min(_workingAreaResolution.y, maxHcCoord.y + 1);
        ++y)
    {
        for (auto x = std::max(0, minHcCoord.x);
            x < std::min(_workingAreaResolution.x, maxHcCoord.x + 1);
            ++x)
        {
            auto cellCenter = getCellWorldCenter({x,y});
            auto distance = glm::length(
                cellCenter - glm::dvec2{toolPosition.x, toolPosition.z}
            );

            if (distance > _toolRadius) { continue; }
            auto gainedHeight = sqrt(
                _toolRadius * _toolRadius - distance * distance
            );

            auto index = _workingAreaResolution.x * y + x;
            auto effectiveCut = toolPosition.y + gainedHeight;
            if (effectiveCut < _checkHeightmap[index])
            {
                objectDamaged = true;
                damageMagnitude = std::max(
                    damageMagnitude,
                    _checkHeightmap[index] - effectiveCut
                );
            }
        }
    }

    return objectDamaged;
}

glm::ivec2 PreciseMillingPathGenerator::getHeightmapCoord(glm::dvec3 position)
{
    glm::dvec2 heightmapCoord {
        position.x / _blockSize.x + 0.5,
        position.z / _blockSize.z + 0.5
    };

    glm::ivec2 memoryCoord {
        static_cast<int>(heightmapCoord.x * _workingAreaResolution.x),
        static_cast<int>(heightmapCoord.y * _workingAreaResolution.y)
    };

    return memoryCoord;
}

glm::dvec2 PreciseMillingPathGenerator::getCellWorldCenter(
    glm::ivec2 coordinate
)
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

void PreciseMillingPathGenerator::filterOutDamages(
    std::vector<std::vector<glm::dvec3>>& output,
    const std::vector<glm::dvec3>& input
)
{
    std::vector<glm::dvec3> buffer;

    for (const auto& position: input)
    {
        if (doesPositionDamageCheckSurface(position))
        {
            if (buffer.size() > 0)
            {
                output.push_back(buffer);
                buffer.clear();
            }
        }
        else
        {
            buffer.push_back(position);
        }
    }

    if (buffer.size() > 0)
    {
        output.push_back(buffer);
        buffer.clear();
    }
}

std::vector<double> PreciseMillingPathGenerator::getScanLineIntersections(
    double constV,
    std::vector<glm::dvec2> boundaries
) const
{
    glm::dvec2 scanStart{};
    glm::dvec2 scanEnd{};

    if (_currentMillingDirection == MillingDirection::LeftToRight)
    {
        scanStart = {-0.1, constV};
        scanEnd = {+1.1, constV};
    }
    else
    {
        scanStart = {constV, -0.1};
        scanEnd = {constV, +1.1};
    }

    std::vector<double> intersectionPoints{0.0, 1.0};

    for (auto i = 0; i+1 < boundaries.size(); ++i)
    {
        auto intersection = fw::intersectSegments<glm::dvec2, double>(
            scanStart,
            scanEnd,
            boundaries[i],
            boundaries[i+1]
        );

        if (intersection.kind == fw::GeometricIntersectionKind::Single)
        {
            auto intersectionPoint = scanStart
                + intersection.t0 * (scanEnd - scanStart);
            std::cout << "int at t0=" << intersection.t0 << std::endl;
            intersectionPoints.push_back(
                _currentMillingDirection == MillingDirection::LeftToRight
                    ? intersectionPoint.x
                    : intersectionPoint.y
            );
        }
        else if (intersection.kind != fw::GeometricIntersectionKind::None)
        {
            std::cout << "intersection: " << (int)intersection.kind << std::endl;
        }
    }

    std::sort(std::begin(intersectionPoints), std::end(intersectionPoints));
    return intersectionPoints;
}


void PreciseMillingPathGenerator::generatePathLine(
    std::vector<glm::dvec3> &path,
    double minU,
    double maxU,
    double constV
)
{
    auto samples = 2 + _maxScanLineResolution;

    auto surfaceCurve = _surface->getConstParameterCurve(
        _currentMillingDirection == MillingDirection::LeftToRight
            ? fw::ParametrizationAxis::V
            : fw::ParametrizationAxis::U,
        constV
    );

    auto normalTransformation = glm::transpose(
        glm::inverse(_surfaceTransformation)
    );

    // todo: fix derivatives
    // hack: there is something wrong with derivatives
    minU = std::max(0.02, minU);
    maxU = std::min(0.98, maxU);

    for (int i = 0; i < samples; ++i)
    {
        auto mixFactor = i / static_cast<double>(samples - 1);
        auto u = glm::mix(minU, maxU, mixFactor);

        glm::vec2 coord{u, constV};
        if (_currentMillingDirection == MillingDirection::TopToBottom)
        {
            std::swap(coord.x, coord.y);
        }

        auto pos = glm::dvec3(_surfaceTransformation
            * glm::dvec4{surfaceCurve->evaluate(u), 1.0});
        auto normal = glm::dvec3(normalTransformation
            * glm::dvec4{_surface->getNormal(coord), 1.0});

        auto centerPos = pos + _toolRadius * glm::normalize(normal);
        path.push_back(centerPos - glm::dvec3(0, _toolRadius, 0));
    }
}

}
