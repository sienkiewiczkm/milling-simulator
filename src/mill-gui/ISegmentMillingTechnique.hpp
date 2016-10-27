#pragma once

#include "CuttingToolParams.hpp"
#include "MillingErrors.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace ms
{

class ISegmentMillingTechnique
{
public:
    virtual ~ISegmentMillingTechnique() = default;

    virtual bool isAvailable(
        glm::dvec3 startPosition,
        glm::dvec3 endPosition
    ) = 0;

    virtual MillingError moveTool(
        std::vector<float> &heightmap,
        glm::ivec2 heightmapResolution,
        glm::mat4 worldHeightmapTransformation,
        float safeHeight,
        const CuttingToolParams &toolParams,
        glm::dvec3 tipStartPosition, 
        glm::dvec3 tipEndPosition,
        bool cuttingProhibited = false
    ) = 0;
};

}
