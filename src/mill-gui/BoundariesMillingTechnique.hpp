#pragma once

#include "ISegmentMillingTechnique.hpp"

namespace ms
{

class BoundariesMillingTechnique:
    public ISegmentMillingTechnique
{
public:
    BoundariesMillingTechnique();
    virtual ~BoundariesMillingTechnique();

    virtual bool isAvailable(glm::dvec3 startPosition, glm::dvec3 endPosition);

    virtual MillingError moveTool(
        std::vector<float> &heightmap,
        glm::ivec2 heightmapResolution,
        glm::mat4 worldHeightmapTransformation,
        float safeHeight,
        const CuttingToolParams &toolParams,
        glm::dvec3 tipStartPosition, 
        glm::dvec3 tipEndPosition,
        bool cuttingProhibited = false
    );

protected:
    static const double cFlatnessEpsilon;

    CuttingToolParams _cuttingToolParams;
    double _tcRadius;

    glm::dmat4 _heightmapTransformation;

    glm::dvec4 getToolBoundaries(glm::dvec3 position);
    glm::dvec4 getAvailableBoundaries(glm::dvec4 bounds);
};

}
