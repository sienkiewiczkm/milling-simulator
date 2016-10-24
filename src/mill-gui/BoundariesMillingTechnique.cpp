#include "ISegmentMillingTechnique.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>

using namespace std;
using namespace glm;

namespace ms
{

BoundariesMillingTechnique::BoundariesMillingTechnique()
{
}

BoundariesMillingTechnique::~BoundariesMillingTechnique()
{
}


MillingError BoundariesMillingTechnique::moveTool(
    std::vector<float> &heightmap,
    glm::ivec2 heightmapResolution,
    glm::mat4 worldHeightmapTransformation,
    float safeHeight,
    const CuttingToolParams &toolParams,
    glm::dvec3 tipStartPosition, 
    glm::dvec3 tipEndPosition
)
{
    _heightmapTransformation = worldHeightmapTransformation;
    _cuttingToolParams = toolParams;

    auto tipTexCoord = worldHeightmapTransformation
        * vec4(tipStartPosition, 1.0f);
    auto htEndPosition = worldHeightmapTransformation
        * vec4(tipEndPosition, 1.0f);

    _tcRadius = ((worldHeightmapTransformation
        * vec4(tipStartPosition + dvec3(toolParams.radius, 0.0, 0.0), 1.0f))
        - tipTexCoord).x;

    auto startBoundaries = getToolBoundaries(tipStartPosition); 
    auto endBoundaries = getToolBoundaries(tipEndPosition); 

    auto texCoordRect = vec4(
        std::min(startBoundaries.x, endBoundaries.x),
        std::min(startBoundaries.y, endBoundaries.y),
        std::max(startBoundaries.z, endBoundaries.z),
        std::max(startBoundaries.w, endBoundaries.w)
    );

    auto boundaries = getAvailableBoundaries(texCoordRect);

    if (boundaries.x > boundaries.z || boundaries.y > boundaries.w)
    {
        return MillingError::None;
    }

    auto minTouched = ivec2(
        static_cast<int>(std::floor(boundaries.x * heightmapResolution.x)),
        static_cast<int>(std::floor(boundaries.y * heightmapResolution.y))
    );

    auto maxTouched = ivec2(
        static_cast<int>(std::ceil(boundaries.z * heightmapResolution.x)),
        static_cast<int>(std::ceil(boundaries.w * heightmapResolution.y))
    );

    auto tcStart = vec2(tipTexCoord.x, tipTexCoord.z);
    auto tcEnd = vec2(htEndPosition.x, htEndPosition.z);
    auto tcSegmentVec = tcEnd - tcStart;
    auto tcSegmentLength = length(tcSegmentVec);

    for (auto y = minTouched.y;
         y <= std::min(maxTouched.y, heightmapResolution.y - 1);
         ++y
        )
    {
        for (auto x = minTouched.x;
             x <= std::min(maxTouched.x, heightmapResolution.x - 1);
             ++x
            )
        {
            auto tcPos = vec2(
                x / static_cast<float>(heightmapResolution.x),
                y / static_cast<float>(heightmapResolution.y)
            );

            auto v = tcPos - tcStart;
            auto projection = dot(v, tcSegmentVec) / tcSegmentLength;
            projection = std::min(tcSegmentLength, std::max(0.0f, projection));
            auto interpolant = projection / tcSegmentLength;
            auto Q = tcStart + interpolant*tcSegmentVec;
            auto interpolatedToolHeight = (1.0-interpolant) * tipStartPosition.y
                + interpolant * tipEndPosition.y;

            auto dist = glm::length(tcPos - Q);

            if (dist > _tcRadius)
                continue;

            auto &cell = heightmap[y*heightmapResolution.x + x];

            auto tcHeight = _tcRadius - sqrt(_tcRadius*_tcRadius - dist*dist);
            tcHeight *= (toolParams.radius / _tcRadius);

            auto toolHeight = interpolatedToolHeight;
            if (toolParams.kind == CuttingToolKind::Ball)
            {
                toolHeight += tcHeight;
            }

            if (cell > toolHeight)
            {
                if (toolHeight < safeHeight)
                {
                    return MillingError::SafeZoneReached;
                }

                if (toolParams.kind == CuttingToolKind::Flat &&
                    abs(tipStartPosition.y - tipEndPosition.y) > 0.001)
                {
                    return MillingError::DrillingHolesWithFlatTool;
                }
            }

            
            cell = std::min(cell, static_cast<float>(toolHeight));
        }
    }

    return MillingError::None;
}

glm::dvec4 BoundariesMillingTechnique::getToolBoundaries(glm::dvec3 position)
{
    auto tcPosition = _heightmapTransformation * dvec4(position, 1.0f);

    auto radiusVector = dvec4(_tcRadius, 0.0, _tcRadius, 0.0);
    auto minTexCoord = tcPosition - radiusVector;
    auto maxTexCoord = tcPosition + radiusVector;

    return glm::dvec4(
        minTexCoord.x, minTexCoord.z, 
        maxTexCoord.x, maxTexCoord.z
    );
}

glm::dvec4 BoundariesMillingTechnique::getAvailableBoundaries(glm::dvec4 bounds)
{
    return glm::dvec4(
        std::max(bounds.x, 0.0),
        std::max(bounds.y, 0.0),
        std::min(bounds.z, 1.0),
        std::min(bounds.w, 1.0)
    );
}

}
