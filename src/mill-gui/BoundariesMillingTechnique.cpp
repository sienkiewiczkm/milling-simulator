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
    auto tipTexCoord = worldHeightmapTransformation
        * vec4(tipStartPosition, 1.0f);

    auto texCoordRadius = ((worldHeightmapTransformation
        * vec4(tipStartPosition + dvec3(toolParams.radius, 0.0, 0.0), 1.0f))
        - tipTexCoord).x;

    auto minTexCoord = tipTexCoord 
        - vec4(texCoordRadius, 0.0, texCoordRadius, 0.0);
    auto maxTexCoord = tipTexCoord
        + vec4(texCoordRadius, 0.0, texCoordRadius, 0.0);
    
    minTexCoord = glm::max(minTexCoord, vec4(0.0f, 0.0f, 0.0f, 0.0f));
    maxTexCoord = glm::min(maxTexCoord, vec4(1.0f, 0.0f, 1.0f, 0.0f));

    ImGui::InputFloat3("Min TexCoord", value_ptr(minTexCoord));
    ImGui::InputFloat3("Max TexCoord", value_ptr(maxTexCoord));
    ImGui::InputFloat("TC Radius", &texCoordRadius);
    
    if (minTexCoord.x > maxTexCoord.x || minTexCoord.z > maxTexCoord.z)
    {
        return;
    }

    auto minTouched = ivec2(
        static_cast<int>(std::floor(minTexCoord.x * heightmapResolution.x)),
        static_cast<int>(std::floor(minTexCoord.z * heightmapResolution.y))
    );

    auto maxTouched = ivec2(
        static_cast<int>(std::ceil(maxTexCoord.x * heightmapResolution.x)),
        static_cast<int>(std::ceil(maxTexCoord.z * heightmapResolution.y))
    );


    auto tcTip = vec2(tipTexCoord.x, tipTexCoord.z);
    for (auto y = minTouched.y; y <= maxTouched.y; ++y)
    {
        for (auto x = minTouched.x; x <= maxTouched.x; ++x)
        {
            auto tcPos = vec2(
                x / static_cast<float>(heightmapResolution.x),
                y / static_cast<float>(heightmapResolution.y)
            );

            auto dist = glm::length(tcPos - tcTip);

            if (dist > texCoordRadius)
                continue;

            auto &cell = heightmap[y*heightmapResolution.x + x];

            auto tcHeight = texCoordRadius
                - sqrt(texCoordRadius*texCoordRadius - dist*dist);
            tcHeight *= (toolParams.radius / texCoordRadius);

            auto toolHeight = tipStartPosition.y;
            if (toolParams.kind == CuttingToolKind::Ball)
            {
                toolHeight += tcHeight;
            }
            
            cell = std::min(cell, static_cast<float>(toolHeight));
        }
    }
}

}
