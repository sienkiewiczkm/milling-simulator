#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "MillPathFormatReader.hpp"

namespace fw
{
    class IParametricSurfaceUV;
}

namespace ms
{

class RoughMillingPathGenerator
{
public:
    RoughMillingPathGenerator();
    virtual ~RoughMillingPathGenerator();

    void setBaseHeight(double baseHeight);
    void setCuttingToolRadius(double radius);
    void setWorkingArea(glm::dvec3 blockSize, glm::dmat4 worldMatrix);
    void setWorkingAreaResolution(glm::ivec2 areaResolution);
    void setSamplingResolution(glm::ivec2 samplingResolution);

    void addParametricSurface(
        std::shared_ptr<fw::IParametricSurfaceUV> surface,
        glm::dmat4 worldMatrix
    );

    void bake();
    std::vector<PathMovement> buildPaths();

protected:
    using SurfaceTransformationTuple =
        std::tuple<std::shared_ptr<fw::IParametricSurfaceUV>, glm::dmat4>;

    void bakeSurfaceIntoHeightmap(const SurfaceTransformationTuple &surface);
    void bakeZigZagPath();
    void simplifyPath();

    glm::dvec2 getCellWorldCenter(glm::ivec2 coordinate);
    double getHighestValueInArea(
        glm::ivec2 minCoordinate,
        glm::ivec2 maxCoordinate
    ) const;

private:
    double _safeHeight;
    double _baseHeight;
    double _additionalOffset;
    double _toolRadius;
    glm::dvec3 _blockSize;
    glm::dmat4 _blockWorldMatrix;
    glm::dmat4 _blockWorldMatrixInv;
    glm::ivec2 _workingAreaResolution;
    glm::ivec2 _samplingResolution;

    std::vector<double> _heightmap;
    std::vector<SurfaceTransformationTuple> _surfaces;
    std::vector<glm::dvec3> _path;
};

}
