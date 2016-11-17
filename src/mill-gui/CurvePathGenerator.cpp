#include "CurvePathGenerator.hpp"

#include <iostream>

namespace ms
{

CurvePathGenerator::CurvePathGenerator():
    _baseHeight{21.5},
    _safeHeight{50.5}
{
}

void CurvePathGenerator::bake()
{
    std::vector<glm::dvec3> buffer;
    for (const auto &curve: _refinementCurves)
    {
        std::cout << "refinement curve found!" << std::endl;
        for (const auto& pos: curve)
        {
            auto transformed = _transform * glm::dvec4{pos, 1.0}
                - glm::dvec4{0, 4.0, 0.0, 0.0};

            std::cout << "refinement position found!" << std::endl;
            if (transformed.y >= _baseHeight)
            {
                buffer.push_back(transformed);
            }
            else
            {
                if (buffer.size() > 1)
                {
                    _rawPaths.push_back(buffer);
                    buffer.clear();
                }
            }
        }

        if (buffer.size() > 0)
        {
            _rawPaths.push_back(buffer);
            buffer.clear();
        }
    }
}

std::vector<PathMovement> CurvePathGenerator::buildPaths()
{
    // todo: remove redundancy (PreciseMillingPathGenerator)
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

}
