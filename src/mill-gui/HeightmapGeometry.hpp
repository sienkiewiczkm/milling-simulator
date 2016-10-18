#pragma once

#include <glad/glad.h>
#include <vector>

struct VertexNormalTexCoords;

class HeightmapGeometry
{
public:
    HeightmapGeometry();
    ~HeightmapGeometry();

    void create(int width, int height);
    void destroy();

    void setHeightmap(const std::vector<float> &heightmap);
    void render();

protected:
    std::vector<VertexNormalTexCoords> generateVertsFromHeightmap(
        const std::vector<float> &heightmap
    );

    std::vector<GLuint> generateIndices();

private:
    bool _skirtEnabled;
    float _skirtLevel;

    int _width, _height;

    float _geometryWidth;
    float _geometryLength;

    GLuint _VAO, _VBO, _EBO;
    GLuint _numElements;
};
