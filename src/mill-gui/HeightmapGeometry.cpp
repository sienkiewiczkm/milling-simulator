#include "HeightmapGeometry.hpp"
#include "Vertices.hpp"

using namespace std;

HeightmapGeometry::HeightmapGeometry()
{
}

HeightmapGeometry::~HeightmapGeometry()
{
}

void HeightmapGeometry::create(
    int width,
    int height,
    const std::vector<float> &heightmap
)
{
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

    vector<VertexNormalTexCoords> vertices;
    vector<GLuint> indices;

    generateGeometryFromHeightmap(width, height, heightmap, vertices, indices);
    _numElements = indices.size();

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(VertexNormalTexCoords),
        &vertices[0],
        GL_DYNAMIC_DRAW
    );

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

void HeightmapGeometry::setHeightmap(
    int width,
    int height,
    const std::vector<float> &heightmap
)
{
}

void HeightmapGeometry::render()
{
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, _numElements, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void HeightmapGeometry::generateGeometryFromHeightmap(
    int width,
    int height,
    const std::vector<float> &heightmap,
    vector<VertexNormalTexCoords> &vertices,
    vector<GLuint> &indices
)
{
    vertices.clear();
    indices.clear();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            vertices.push_back(VertexNormalTexCoords(
                glm::vec3(
                    x/(float)width-0.5f,
                    heightmap[width*y+x],
                    y/(float)height-0.5f
                ),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec2(x/(float)width, y/(float)height)
            ));
        }
    }

    for (int y = 0; y < height - 1; ++y)
    {
        for (int x = 0; x < width - 1; ++x)
        {
            int currentIndex = y*width+x;
            int nextRowIndex = currentIndex + width;

            indices.push_back(currentIndex);
            indices.push_back(currentIndex+1);
            indices.push_back(nextRowIndex);

            indices.push_back(nextRowIndex);
            indices.push_back(currentIndex+1);
            indices.push_back(nextRowIndex+1);
        }
    }
}
