#pragma once

#include "IParametricSurfaceUV.hpp"
#include "MillPathFormatReader.hpp"
#include "glm/glm.hpp"
#include <memory>
#include <vector>

namespace ms
{

enum class MillingDirection
{
    LeftToRight,
    TopToBottom,
};

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

    void clearCheckSurfaces();

    void addCheckSurface(
        std::shared_ptr<fw::IParametricSurfaceUV> surface,
        const glm::dmat4& surfaceTransformation
    );

    void setBaseHeight(double baseHeight);
    void setCuttingToolRadius(double radius);
    void setWorkingArea(glm::dvec3 blockSize, glm::dmat4 worldMatrix);
    void setWorkingAreaResolution(glm::ivec2 areaResolution);
    void setSamplingResolution(glm::ivec2 samplingResolution);
    void setMaximumHeightmapShift(double maxHeightmapShift);
    void setHeightmapShiftOffset(double heightmapShiftOffset);

    void setParametricSurfaceBoundaries(
        const std::vector<glm::dvec2>& boundaries
    );

    void bake(
        bool inverseTrimmingSide = false,
        MillingDirection direction = MillingDirection::LeftToRight
    );
    std::vector<PathMovement> buildPaths();

protected:
    void bakeCheckSurfaceHeightmap();
    void bakePositionOnCheckHeightmap(const glm::dvec3& position);
    bool doesPositionDamageCheckSurface(
        const glm::dvec3& toolPosition,
        double &outDamageMagnitude
    );
    glm::ivec2 getHeightmapCoord(glm::dvec3 position);
    glm::dvec2 getCellWorldCenter(glm::ivec2 coordinate);

    void filterOutDamages(
        std::vector<std::vector<glm::dvec3>>& output,
        const std::vector<glm::dvec3>& input
    );

    std::vector<double> getScanLineIntersections(
        double constV,
        std::vector<glm::dvec2> boundaries
    ) const;

    void generatePathLine(
        std::vector<glm::dvec3> &path,
        double minU,
        double maxU,
        double constV
    );

    MillingDirection _currentMillingDirection;

private:
    double _baseHeight;
    double _safeHeight;
    double _toolRadius;
    double _heightmapShiftOffset;
    double _maxHeightmapShift;
    glm::dvec2 _toolHeightmapRadius;
    int _numScanLines;
    int _maxScanLineResolution;

    glm::dvec3 _blockSize;
    glm::dmat4 _blockWorldMatrix;
    glm::dmat4 _blockWorldMatrixInv;
    glm::ivec2 _workingAreaResolution;
    glm::ivec2 _samplingResolution;
    std::vector<double> _checkHeightmap;

    std::vector<std::vector<glm::dvec3>> _rawPaths;

    std::shared_ptr<fw::IParametricSurfaceUV> _surface;
    glm::dmat4 _surfaceTransformation;
    std::vector<glm::dvec2> _boundaries;

    std::vector<std::shared_ptr<fw::IParametricSurfaceUV>> _checkSurface;
    std::vector<glm::dmat4> _checkSurfaceTransformation;
};

}

