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
