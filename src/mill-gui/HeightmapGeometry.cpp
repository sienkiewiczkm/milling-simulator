#include "HeightmapGeometry.hpp"
#include "Vertices.hpp"
#include <cassert>

using namespace std;

HeightmapGeometry::HeightmapGeometry()
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

    for (int y = 0; y < _height; ++y)
    {
        for (int x = 0; x < _width; ++x)
        {
            vertices.push_back(VertexNormalTexCoords(
                glm::vec3(
                    x/(float)_width-0.5f,
                    heightmap[_width*y+x],
                    y/(float)_height-0.5f
                ),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec2(x/(float)_width, y/(float)_height)
            ));
        }
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

    return indices;
}
