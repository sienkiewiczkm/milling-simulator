#pragma once

#include "glm/glm.hpp"

#include "MillPathFormatReader.hpp"

namespace ms
{

class CurvePathGenerator
{
public:
    CurvePathGenerator();

    inline void setBaseHeight(double baseHeight) { _baseHeight = baseHeight; }
    inline void setRefinementCurves(
        const std::vector<std::vector<glm::dvec3>>& refinementCurves
    );
    inline void setTransform(glm::dmat4 transform) { _transform = transform; }

    void bake();
    std::vector<PathMovement> buildPaths();

private:
    double _baseHeight;
    double _safeHeight;
    glm::dmat4 _transform;
    std::vector<std::vector<glm::dvec3>> _refinementCurves;
    std::vector<std::vector<glm::dvec3>> _rawPaths;
};

void CurvePathGenerator::setRefinementCurves(
    const std::vector<std::vector<glm::dvec3>>& refinementCurves
)
{
    _refinementCurves = refinementCurves;
}

}
