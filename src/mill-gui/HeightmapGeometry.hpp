#pragma once

#include "OpenGLHeaders.hpp"
#include "Mesh.hpp"
#include "Vertices.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <memory>

struct VertexNormalTexCoords;

class HeightmapGeometry
{
public:
    HeightmapGeometry();
    ~HeightmapGeometry();

    void create(int width, int length, glm::vec3 size);
    void destroy();

    void setHeightmap(const std::vector<float> &heightmap);
    void render();

protected:
    void generateHeightmapGeometry();

private:
    bool _skirtEnabled;
    int _width, _length;
    glm::vec3 _size;

    std::shared_ptr<Mesh<Vertex3DNormal2TexCoord>> _mesh;
};
