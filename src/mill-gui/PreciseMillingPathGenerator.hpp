#pragma once

#include "IParametricSurfaceUV.hpp"
#include "MillPathFormatReader.hpp"
#include "glm/glm.hpp"
#include <memory>
#include <vector>

namespace ms
{

class PreciseMillingPathGenerator
{
public:
    PreciseMillingPathGenerator();
    ~PreciseMillingPathGenerator();

    void setNumScanLines(int scanLines);
    void setNumLineMaximumResolution(int maxScanLineResolution);

    void setParametricSurface(
        std::shared_ptr<fw::IParametricSurfaceUV> surface,
        const glm::dmat4 &surfaceTransformation
    );

    void setParametricSurfaceBoundaries(
        const std::vector<glm::dvec2>& boundaries
    );

    void bake();
    std::vector<PathMovement> buildPaths();

protected:
    std::vector<double> getScanLineIntersections(double constV) const;
    void generatePathLine(
        std::vector<glm::dvec3> &path,
        double minU,
        double maxU,
        double constV
    );

private:
    double _baseHeight;
    double _safeHeight;
    double _toolRadius;
    int _numScanLines;
    int _maxScanLineResolution;

    std::vector<std::vector<glm::dvec3>> _rawPaths;

    std::shared_ptr<fw::IParametricSurfaceUV> _surface;
    glm::dmat4 _surfaceTransformation;

    std::vector<glm::dvec2> _boundaries;
};

}

