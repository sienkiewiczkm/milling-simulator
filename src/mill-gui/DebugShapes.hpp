#pragma once

#include "Mesh.hpp"
#include "Vertices.hpp"

#include <glad/glad.h>

#include <vector>

Mesh<VertexNormalTexCoords> createPlane(float width, float length);
std::vector<GLfloat> createCube(float width, float height, float length);

Mesh<VertexNormalTexCoords> createCylinder(
    float height, float radius, float circleSubdivisions
);

Mesh<VertexNormalTexCoords> createSphere(
    float radius,
    int latitudeSubdivisions,
    int longtitudeSubdivisions
);
