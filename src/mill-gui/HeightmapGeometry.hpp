#pragma once

#include <glad/glad.h>
#include <vector>

struct VertexNormalTexCoords;

class HeightmapGeometry
{
public:
    HeightmapGeometry();
    ~HeightmapGeometry();

    void create(int width, int height, 
        const std::vector<float> &heightmap);
    void destroy();

    void setHeightmap(int width, int height, 
        const std::vector<float> &heightmap);
    void render();

protected:
    void generateGeometryFromHeightmap(
        int width,
        int height,
        const std::vector<float> &heightmap,
        std::vector<VertexNormalTexCoords> &vertices,
        std::vector<GLuint> &indices
    );

private:
    float _geometryWidth;
    float _geometryLength;

    GLuint _VAO, _VBO, _EBO;
    GLuint _numElements;
};
