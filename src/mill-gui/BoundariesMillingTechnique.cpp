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


void BoundariesMillingTechnique::moveTool(
    std::vector<float> &heightmap,
    glm::ivec2 heightmapResolution,
    glm::mat4 worldHeightmapTransformation,
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
    auto endBoundaries = getToolBoundaries(tipStartPosition); 

    auto minTexCoord = vec4(
        std::min(startBoundaries.x, endBoundaries.x), 0.0f,
        std::min(startBoundaries.y, endBoundaries.y), 0.0f
    );

    auto maxTexCoord = vec4(
        std::min(startBoundaries.z, endBoundaries.z), 0.0f,
        std::min(startBoundaries.w, endBoundaries.w), 0.0f
    );

    auto minTouched = ivec2(
        static_cast<int>(std::floor(minTexCoord.x * heightmapResolution.x)),
        static_cast<int>(std::floor(minTexCoord.z * heightmapResolution.y))
    );

    auto maxTouched = ivec2(
        static_cast<int>(std::ceil(maxTexCoord.x * heightmapResolution.x)),
        static_cast<int>(std::ceil(maxTexCoord.z * heightmapResolution.y))
    );

    auto tcStart = vec2(tipTexCoord.x, tipTexCoord.z);
    auto tcEnd = vec2(htEndPosition.x, htEndPosition.z);
    auto tcSegmentVec = tcEnd - tcStart;
    auto tcSegmentLength = length(tcSegmentVec);

    for (auto y = minTouched.y; y <= maxTouched.y; ++y)
    {
        for (auto x = minTouched.x; x <= maxTouched.x; ++x)
        {
            auto tcPos = vec2(
                x / static_cast<float>(heightmapResolution.x),
                y / static_cast<float>(heightmapResolution.y)
            );

            // find closest point on 
            auto v = tcPos - tcStart;
            auto projection = dot(v, tcSegmentVec) / tcSegmentLength;
            projection = std::min(tcSegmentLength, std::max(0.0f, projection));
            auto Q = tcStart + (projection/tcSegmentLength)*tcSegmentVec;

            auto dist = glm::length(tcPos - Q);

            if (dist > _tcRadius)
                continue;

            auto &cell = heightmap[y*heightmapResolution.x + x];

            auto tcHeight = _tcRadius - sqrt(_tcRadius*_tcRadius - dist*dist);
            tcHeight *= (toolParams.radius / _tcRadius);

            auto toolHeight = tipStartPosition.y;
            if (toolParams.kind == CuttingToolKind::Ball)
            {
                toolHeight += tcHeight;
            }
            
            cell = std::min(cell, static_cast<float>(toolHeight));
        }
    }
}

glm::dvec4 BoundariesMillingTechnique::getToolBoundaries(glm::dvec3 position)
{
    auto tcPosition = _heightmapTransformation * dvec4(position, 1.0f);
    auto minTexCoord = tcPosition - dvec4(_tcRadius, 0.0, _tcRadius, 0.0);
    auto maxTexCoord = tcPosition + dvec4(_tcRadius, 0.0, _tcRadius, 0.0);
    minTexCoord = glm::max(minTexCoord, dvec4(0.0f, 0.0f, 0.0f, 0.0f));
    maxTexCoord = glm::min(maxTexCoord, dvec4(1.0f, 0.0f, 1.0f, 0.0f));

    return glm::dvec4(
        minTexCoord.x, minTexCoord.z, 
        maxTexCoord.x, maxTexCoord.z
    );
}

}
