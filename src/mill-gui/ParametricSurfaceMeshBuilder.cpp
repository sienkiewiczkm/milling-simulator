#include "ParametricSurfaceMeshBuilder.hpp"
#include "IParametricSurfaceUV.hpp"
#include "Mesh.hpp"
#include "Vertices.hpp"

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

namespace fw
{

ParametricSurfaceMeshBuilder::ParametricSurfaceMeshBuilder()
{
}

ParametricSurfaceMeshBuilder::~ParametricSurfaceMeshBuilder()
{
}

void ParametricSurfaceMeshBuilder::setSamplingResolution(
    glm::ivec2 samplingResolution
)
{
    _samplingResolution = samplingResolution;
}

glm::ivec2 ParametricSurfaceMeshBuilder::getSamplingResolution() const
{
    return _samplingResolution;
}

std::shared_ptr<Mesh<VertexNormalTexCoords>>
        ParametricSurfaceMeshBuilder::build(
    std::shared_ptr<IParametricSurfaceUV> surface,
    glm::dvec2 minimumParameter,
    glm::dvec2 maximumParameter
) const
{
    std::vector<VertexNormalTexCoords> vertices;
    std::vector<GLuint> indices;

    vertices.reserve(_samplingResolution.x * _samplingResolution.y);

    for (auto y = 0; y < _samplingResolution.y; ++y)
    {
        auto dy = static_cast<double>(y)/(_samplingResolution.y - 1);
        for (auto x = 0; x < _samplingResolution.x; ++x)
        {
            auto dx = static_cast<double>(x)/(_samplingResolution.x - 1);
            glm::dvec2 dp { dx, dy };
            glm::dvec2 parametrisation =
                glm::mix(minimumParameter, maximumParameter, dp);

            auto position = surface->getPosition(parametrisation);
            auto normal = surface->getNormal(parametrisation);

            vertices.push_back({
                glm::vec3(position),
                glm::vec3(normal),
                glm::vec2(dp)
            });
        }
    }

    for (auto y = 0; y < _samplingResolution.y - 1; ++y)
    {
        for (auto x = 0; x < _samplingResolution.x - 1; ++x)
        {
            auto baseIndex = y * _samplingResolution.x + x;
            indices.push_back(baseIndex);
            indices.push_back(baseIndex+1);
            indices.push_back(baseIndex+_samplingResolution.x);

            indices.push_back(baseIndex+1);
            indices.push_back(baseIndex+_samplingResolution.x);
            indices.push_back(baseIndex+_samplingResolution.x+1);
        }
    }

    return std::make_shared<Mesh<VertexNormalTexCoords>>(vertices, indices);
}

}
