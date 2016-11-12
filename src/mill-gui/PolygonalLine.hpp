#pragma once

#include "Vertices.hpp"
#include <vector>

namespace fw
{

class PolygonalLine
{
public:
    PolygonalLine(const std::vector<VertexColor> &linePoints);
    virtual ~PolygonalLine();

    void render() const;

protected:
    GLuint _vao, _vbo;
    int _numElements;

    void createBuffers(const std::vector<VertexColor> &linePoints);
};

};
