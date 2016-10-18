#include "HeightmapGeometry.hpp"
#include "Vertices.hpp"
#include <cassert>

using namespace std;
using namespace glm;

HeightmapGeometry::HeightmapGeometry() :
    _skirtEnabled(true),
    _skirtLevel(0.0f)
{
}

HeightmapGeometry::~HeightmapGeometry()
{
}

void HeightmapGeometry::create(int width, int height)
{
    _width = width;
    _height = height;

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

    vector<GLuint> indices = generateIndices();
    _numElements = indices.size();

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        &indices[0],
        GL_STATIC_DRAW
    );

    VertexNormalTexCoords::setupAttribPointers();

    glBindVertexArray(0);
}

void HeightmapGeometry::destroy()
{
}

void HeightmapGeometry::setHeightmap(const std::vector<float> &heightmap)
{
    assert(heightmap.size() == _width * _height);
    
    auto vertices = generateVertsFromHeightmap(heightmap);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(VertexNormalTexCoords),
        &vertices[0],
        GL_DYNAMIC_DRAW
    );

    glBindVertexArray(0);
}

void HeightmapGeometry::render()
{
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, _numElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

vector<VertexNormalTexCoords> HeightmapGeometry::generateVertsFromHeightmap(
    const std::vector<float> &heightmap
)
{
    vector<VertexNormalTexCoords> vertices;
    vertices.reserve(_width * _height);

    for (int y = 0; y < _height; ++y)
    {
        for (int x = 0; x < _width; ++x)
        {
            vertices.push_back(VertexNormalTexCoords(
                glm::vec3(
                    x/((float)_width-1)-0.5f,
                    heightmap[_width*y+x],
                    y/((float)_height-1)-0.5f
                ),
                glm::vec3(0.0f, 0.0f, 0.0f),
                glm::vec2(x/(float)_width, y/(float)_height)
            ));
        }
    }

    for (int y = 0; y < _height - 1; ++y)
    {
        for (int x = 0; x < _width - 1; ++x)
        {
            auto i1 = _width*y+x;
            auto i2 = _width*y+x+1;
            auto i3 = _width*(y+1)+x;
            auto i4 = _width*(y+1)+x+1;

            auto v1 = vertices[i3].position - vertices[i1].position;
            auto v2 = vertices[i2].position - vertices[i1].position;
            auto v3 = vertices[i2].position - vertices[i4].position;
            auto v4 = vertices[i3].position - vertices[i4].position;

            auto normal1 = glm::cross(v1, v2);
            auto normal2 = glm::cross(v3, v4);

            vertices[i1].normal += normal1;
            vertices[i2].normal += normal1;
            vertices[i3].normal += normal1;

            vertices[i2].normal += normal2;
            vertices[i3].normal += normal2;
            vertices[i4].normal += normal2;
        }
    }

    for (int y = 0; y < _height; ++y)
    {
        for (int x = 0; x < _width; ++x)
        {
            if (glm::length(vertices[_width*y+x].normal) == 0.0f)
                continue;

            vertices[_width*y+x].normal =
                glm::normalize(vertices[_width*y+x].normal);
        }
    }

    if (!_skirtEnabled) { return vertices; }

    /* todo: cleanup this code */
    for (int x = 0; x < _width; ++x)
    {
        float progress = x / static_cast<float>(_width-1);
        float worldX = -0.5f + progress;

        vec3 position1 = vec3(worldX, _skirtLevel, -0.5f);
        vec3 value1 = vec3(worldX, heightmap[x], -0.5f);
        vec3 normal1 = vec3(0.0f, 0.0f, -1.0f);
        vec2 texcoord11 = vec2(progress, 0.0f);
        vec2 texcoord12 = vec2(progress, 1.0f);

        vertices.push_back(VertexNormalTexCoords(
            position1, normal1, texcoord11
        ));

        vertices.push_back(VertexNormalTexCoords(
            value1, normal1, texcoord12
        ));

        vec3 position2 = vec3(worldX, _skirtLevel, +0.5f);
        vec3 value2 = vec3(worldX, heightmap[(_height-1)*_width+x], +0.5f);
        vec3 normal2 = vec3(0.0f, 0.0f, 1.0f);
        vec2 texcoord21 = vec2(progress, 0.0f);
        vec2 texcoord22 = vec2(progress, 1.0f);

        vertices.push_back(VertexNormalTexCoords(
            position2, normal2, texcoord21
        ));

        vertices.push_back(VertexNormalTexCoords(
            value2, normal2, texcoord22
        ));
    }

    for (int y = 0; y < _height; ++y)
    {
        float progress = y / static_cast<float>(_height-1);
        float worldZ = -0.5f + progress;

        vec3 position1 = vec3(-0.5f, _skirtLevel, worldZ);
        vec3 value1 = vec3(-0.5f, heightmap[y*_width], worldZ);
        vec3 normal1 = vec3(-1.0f, 0.0f, 0.0f);
        vec2 texcoord11 = vec2(progress, 0.0f);
        vec2 texcoord12 = vec2(progress, 1.0f);

        vertices.push_back(VertexNormalTexCoords(
            position1, normal1, texcoord11
        ));

        vertices.push_back(VertexNormalTexCoords(
            value1, normal1, texcoord12
        ));

        vec3 position2 = vec3(+0.5f, _skirtLevel, worldZ);
        vec3 value2 = vec3(+0.5f, heightmap[y*_width+_width-1], worldZ);
        vec3 normal2 = vec3(+1.0f, 0.0f, 0.0f);
        vec2 texcoord21 = vec2(progress, 0.0f);
        vec2 texcoord22 = vec2(progress, 1.0f);

        vertices.push_back(VertexNormalTexCoords(
            position2, normal2, texcoord21
        ));

        vertices.push_back(VertexNormalTexCoords(
            value2, normal2, texcoord22
        ));
    }

    return vertices;
}

std::vector<GLuint> HeightmapGeometry::generateIndices()
{
    std::vector<GLuint> indices;

    for (int y = 0; y < _height - 1; ++y)
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

    if (!_skirtEnabled) { return indices; }

    int skirtBase = _width * _height;
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
    for (int y = 0; y < _height - 1; ++y)
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

    return indices;
}
