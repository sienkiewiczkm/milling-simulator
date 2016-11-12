#include "PolygonalLine.hpp"

namespace fw
{

PolygonalLine::PolygonalLine(const std::vector<VertexColor> &linePoints)
{
    createBuffers(linePoints);
}

PolygonalLine::~PolygonalLine()
{
}

void PolygonalLine::render() const
{
    glBindVertexArray(_vao);
    glDrawArrays(GL_LINE_STRIP, 0, _numElements);
    glBindVertexArray(0);
}

void PolygonalLine::createBuffers(const std::vector<VertexColor> &linePoints)
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        linePoints.size() * sizeof(VertexColor),
        linePoints.data(),
        GL_STATIC_DRAW
    );

    VertexColor::setupAttribPointers();

    glBindVertexArray(0);

    _numElements = linePoints.size();
}

}
