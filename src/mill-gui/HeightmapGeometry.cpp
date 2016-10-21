#include "HeightmapGeometry.hpp"
#include "Vertices.hpp"
#include <cassert>

using namespace std;
using namespace glm;

HeightmapGeometry::HeightmapGeometry() :
    _skirtEnabled(true)
{
}

HeightmapGeometry::~HeightmapGeometry()
{
}

void HeightmapGeometry::create(int width, int length, vec3 size)
{
    _width = width;
    _length = length;
    _size = size;

    generateHeightmapGeometry();
}

void HeightmapGeometry::destroy()
{
}

void HeightmapGeometry::setHeightmap(const std::vector<float> &heightmap)
{
    assert(heightmap.size() == _width * _length);
}

void HeightmapGeometry::render()
{
    _mesh->render();
}

void HeightmapGeometry::generateHeightmapGeometry()
{
    vector<Vertex3DNormal2TexCoord> vertices;
    vertices.reserve(_width * _length);

    for (int y = 0; y < _length; ++y)
    {
        for (int x = 0; x < _width; ++x)
        {
            vertices.push_back(Vertex3DNormal2TexCoord(
                glm::vec3(
                    x/((float)_width-1)-0.5f,
                    0.0f,
                    y/((float)_length-1)-0.5f
                ),
                glm::vec3(),
                glm::vec2(x/(float)_width, y/(float)_length),
                glm::vec2(1.0f, 1.0f)
            ));
        }
    }

    for (int x = 0; x < _width; ++x)
    {
        float progress = x / static_cast<float>(_width-1);
        float worldX = -0.5f + progress;

        vec3 position1 = vec3(worldX, 0.0f, -0.5f);
        vec3 value1 = vec3(worldX, 0.0f, -0.5f);
        vec3 normal1 = vec3(0.0f, 0.0f, -0.0f); // -1
        vec2 texcoord11 = vec2(progress, 1.0f);
        vec2 texcoord12 = vec2(progress, 0.0f);

        vertices.push_back(Vertex3DNormal2TexCoord(
            position1, normal1, texcoord11, vec2(0.0f, 0.0f)
        ));

        vertices.push_back(Vertex3DNormal2TexCoord(
            value1, normal1, texcoord12, vec2(1.0f, 0.0f)
        ));

        vec3 position2 = vec3(worldX, 0.0f, +0.5f);
        vec3 value2 = vec3(worldX, 0.0f, +0.5f);
        vec3 normal2 = vec3(0.0f, 0.0f, 0.0f); //z=1
        vec2 texcoord21 = vec2(progress, 0.0f);
        vec2 texcoord22 = vec2(progress, 1.0f);

        vertices.push_back(Vertex3DNormal2TexCoord(
            position2, normal2, texcoord21, vec2(0.0f, 0.0f)
        ));

        vertices.push_back(Vertex3DNormal2TexCoord(
            value2, normal2, texcoord22, vec2(1.0f, 0.0f)
        ));
    }

    for (int y = 0; y < _length; ++y)
    {
        float progress = y / static_cast<float>(_length-1);
        float worldZ = -0.5f + progress;

        vec3 position1 = vec3(-0.5f, 0.0f, worldZ);
        vec3 value1 = vec3(-0.5f, 0.0f, worldZ);
        vec3 normal1 = vec3(-1.0f, 0.0f, 0.0f);
        vec2 texcoord11 = vec2(1.0f, progress);
        vec2 texcoord12 = vec2(0.0f, progress);

        vertices.push_back(Vertex3DNormal2TexCoord(
            position1, normal1, texcoord11, vec2(0.0f, 0.0f)
        ));

        vertices.push_back(Vertex3DNormal2TexCoord(
            value1, normal1, texcoord12, vec2(1.0f, 0.0f)
        ));

        vec3 position2 = vec3(+0.5f, 0.0f, worldZ);
        vec3 value2 = vec3(+0.5f, 0.0f, worldZ);
        vec3 normal2 = vec3(+1.0f, 0.0f, 0.0f);
        vec2 texcoord21 = vec2(0.0f, progress);
        vec2 texcoord22 = vec2(1.0f, progress);

        vertices.push_back(Vertex3DNormal2TexCoord(
            position2, normal2, texcoord21, vec2(0.0f, 0.0f)
        ));

        vertices.push_back(Vertex3DNormal2TexCoord(
            value2, normal2, texcoord22, vec2(1.0f, 0.0f)
        ));
    }

    std::vector<GLuint> indices;

    for (int y = 0; y < _length - 1; ++y)
    {
        for (int x = 0; x < _width - 1; ++x)
        {
            int currentIndex = y*_width+x;
            int nextRowIndex = currentIndex + _width;

            indices.push_back(currentIndex);
            indices.push_back(currentIndex+1);
            indices.push_back(nextRowIndex);

            indices.push_back(nextRowIndex);
            indices.push_back(currentIndex+1);
            indices.push_back(nextRowIndex+1);
        }
    }

    int skirtBase = _width * _length;
    for (int x = 0; x < _width - 1; ++x)
    {
        indices.push_back(skirtBase + 4*x);
        indices.push_back(skirtBase + 4*(x + 1));
        indices.push_back(skirtBase + 4*x + 1);

        indices.push_back(skirtBase + 4*(x + 1));
        indices.push_back(skirtBase + 4*x + 1);
        indices.push_back(skirtBase + 4*(x + 1) + 1);

        indices.push_back(skirtBase + 4*x + 2);
        indices.push_back(skirtBase + 4*(x + 1) + 2);
        indices.push_back(skirtBase + 4*x + 1 + 2);

        indices.push_back(skirtBase + 4*(x + 1) + 2);
        indices.push_back(skirtBase + 4*x + 1 + 2);
        indices.push_back(skirtBase + 4*(x + 1) + 1 + 2);
    }

    skirtBase = skirtBase + 4*_width;
    for (int y = 0; y < _length - 1; ++y)
    {
        indices.push_back(skirtBase + 4*y);
        indices.push_back(skirtBase + 4*(y + 1));
        indices.push_back(skirtBase + 4*y + 1);

        indices.push_back(skirtBase + 4*(y + 1));
        indices.push_back(skirtBase + 4*y + 1);
        indices.push_back(skirtBase + 4*(y + 1) + 1);

        indices.push_back(skirtBase + 4*y + 2);
        indices.push_back(skirtBase + 4*(y + 1) + 2);
        indices.push_back(skirtBase + 4*y + 1 + 2);

        indices.push_back(skirtBase + 4*(y + 1) + 2);
        indices.push_back(skirtBase + 4*y + 1 + 2);
        indices.push_back(skirtBase + 4*(y + 1) + 1 + 2);
    }

    _mesh = make_shared<Mesh<Vertex3DNormal2TexCoord>>(vertices, indices);
}
