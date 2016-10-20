#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct VertexNormalTexCoords
{
    VertexNormalTexCoords();
    VertexNormalTexCoords(
        glm::vec3 position,
        glm::vec3 normal,
        glm::vec2 texCoords
    );

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    static void setupAttribPointers();
};

struct Vertex3D2TexCoord
{
    Vertex3D2TexCoord();
    Vertex3D2TexCoord(
        glm::vec3 position,
        glm::vec2 texCoord1,
        glm::vec2 texCoord2
    );

    glm::vec3 position;
    glm::vec2 texCoord1;
    glm::vec2 texCoord2;

    static void setupAttribPointers();
};
