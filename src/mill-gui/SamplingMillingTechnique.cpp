#include "SamplingMillingTechnique.hpp"

#include <iostream>
#include <imgui.h>

using namespace glm;

namespace ms
{

SamplingMillingTechnique::SamplingMillingTechnique()
{
}

SamplingMillingTechnique::~SamplingMillingTechnique()
{
}

bool SamplingMillingTechnique::isAvailable(
    glm::dvec3 startPosition,
    glm::dvec3 endPosition
)
{
    return true;
}

MillingError SamplingMillingTechnique::moveTool(
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

    auto tcStart = vec2(tipTexCoord.x, tipTexCoord.z);
    auto tcEnd = vec2(htEndPosition.x, htEndPosition.z);

    auto samples = static_cast<int>(
        1.25f * glm::length(tcStart - tcEnd) *
            std::max(heightmapResolution.x, heightmapResolution.y)
    );

    samples += 20;

    for (auto i = 0; i < samples; ++i)
    {
        auto progress = i/static_cast<double>(samples-1);
        auto tipPosition = (1.0f - progress) * tipStartPosition
            + progress * tipEndPosition;

        auto tcToolPosition4 = worldHeightmapTransformation
            * vec4(tipPosition, 1.0f);
        auto tcToolPosition = vec2(tcToolPosition4.x, tcToolPosition4.z);

        auto sampleBoundaries = getToolBoundaries(tipPosition); 
        auto boundaries = getAvailableBoundaries(sampleBoundaries);

        if (boundaries.x > boundaries.z || boundaries.y > boundaries.w)
        {
            continue;
        }

        auto minTouched = ivec2(
            static_cast<int>(std::floor(boundaries.x * heightmapResolution.x)),
            static_cast<int>(std::floor(boundaries.y * heightmapResolution.y))
        );

        auto maxTouched = ivec2(
            static_cast<int>(std::ceil(boundaries.z * heightmapResolution.x)),
            static_cast<int>(std::ceil(boundaries.w * heightmapResolution.y))
        );

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

                auto dist = glm::length(tcPos - tcToolPosition);

                if (dist > _tcRadius)
                    continue;

                auto toolHeight = tipPosition.y;

                auto &cell = heightmap[y*heightmapResolution.x + x];

                auto tcHeight = _tcRadius - sqrt(_tcRadius*_tcRadius - dist*dist);
                tcHeight *= (toolParams.radius / _tcRadius);

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
                        tipStartPosition.y - 0.001 > tipEndPosition.y)
                    {
                        return MillingError::DrillingHolesWithFlatTool;
                    }
                }

                cell = std::min(cell, static_cast<float>(toolHeight));
            }
        }
    }
    
    return MillingError::None;
}

glm::dvec4 SamplingMillingTechnique::getToolBoundaries(glm::dvec3 position)
{
    // todo: remove code duplication (BoundariesMillingTechnique.cpp)
    auto tcPosition = _heightmapTransformation * dvec4(position, 1.0f);

    auto radiusVector = dvec4(_tcRadius, 0.0, _tcRadius, 0.0);
    auto minTexCoord = tcPosition - radiusVector;
    auto maxTexCoord = tcPosition + radiusVector;

    return glm::dvec4(
        minTexCoord.x, minTexCoord.z, 
        maxTexCoord.x, maxTexCoord.z
    );
}

glm::dvec4 SamplingMillingTechnique::getAvailableBoundaries(glm::dvec4 bounds)
{
    // todo: remove code duplication (BoundariesMillingTechnique.cpp)
    return glm::dvec4(
        std::max(bounds.x, 0.0),
        std::max(bounds.y, 0.0),
        std::min(bounds.z, 1.0),
        std::min(bounds.w, 1.0)
    );
}

}
