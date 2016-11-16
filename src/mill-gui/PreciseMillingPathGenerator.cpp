#include "PreciseMillingPathGenerator.hpp"
#include "GeometricIntersections.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include <iostream>

namespace ms
{

PreciseMillingPathGenerator::PreciseMillingPathGenerator():
    _safeHeight{50.5},
    _baseHeight{20.5},
    _numScanLines{64},
    _maxScanLineResolution{64},
    _toolRadius{4.0}
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

void PreciseMillingPathGenerator::setParametricSurfaceBoundaries(
    const std::vector<glm::dvec2>& boundaries
)
{
    _boundaries = boundaries;
}

void PreciseMillingPathGenerator::bake()
{
    _rawPaths.clear();
    for (auto row = 0; row < _numScanLines; ++row)
    {
        auto v = row / static_cast<double>(_numScanLines - 1);
        auto scanPoints = getScanLineIntersections(v);

        if (scanPoints.size()%2 == 1)
        {
            std::cout << "Whoopsie. There is odd numer of intersections. "
                << "Skipping." << std::endl;
            continue;
        }

        for (auto i = 1; i < scanPoints.size(); i += 2)
        {
            std::vector<glm::dvec3> currentPath;
            generatePathLine(currentPath, scanPoints[i], scanPoints[i+1], v);
            _rawPaths.push_back(currentPath);
        }
    }

}

std::vector<PathMovement> PreciseMillingPathGenerator::buildPaths()
{
    std::vector<PathMovement> movements;

    for (const auto &path: _rawPaths)
    {
        if (path.size() == 0) { continue; }

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

std::vector<double> PreciseMillingPathGenerator::getScanLineIntersections(
    double constV
) const
{
    glm::dvec2 scanStart{-0.1, constV};
    glm::dvec2 scanEnd{+1.1, constV};

    std::vector<double> intersectionPoints;

    for (auto i = 0; i+1 < _boundaries.size(); ++i)
    {
        auto intersection = fw::intersectSegments<glm::dvec2, double>(
            scanStart,
            scanEnd,
            _boundaries[i],
            _boundaries[i+1]
        );

        if (intersection.kind == fw::GeometricIntersectionKind::Single)
        {
            auto intersectionPoint = scanStart
                + intersection.t0 * (scanEnd - scanStart);
            intersectionPoints.push_back(intersectionPoint.x);
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
        fw::ParametrizationAxis::V,
        constV
    );

    auto normalTransformation = glm::transpose(
        glm::inverse(_surfaceTransformation)
    );

    // todo: fix derivatives
    // hack: there is something wrong with derivatives
    minU = std::max(0.01, minU);
    maxU = std::min(0.99, maxU);

    for (int i = 0; i < samples; ++i)
    {
        auto mixFactor = i / static_cast<double>(samples - 1);
        auto u = glm::mix(minU, maxU, mixFactor);

        auto pos = glm::dvec3(_surfaceTransformation
            * glm::dvec4{surfaceCurve->evaluate(u), 1.0});
        auto normal = glm::dvec3(normalTransformation
            * glm::dvec4{_surface->getNormal({u, constV}), 1.0});

        auto centerPos = pos + _toolRadius * glm::normalize(normal);
        path.push_back(centerPos - glm::dvec3(0, _toolRadius, 0));
    }
}

}
