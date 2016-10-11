#include "Vertices.hpp"

VertexNormalTexCoords::VertexNormalTexCoords()
{
}

VertexNormalTexCoords::VertexNormalTexCoords(
    glm::vec3 position,
    glm::vec3 normal,
    glm::vec2 texCoords
) : position(position), normal(normal), texCoords(texCoords)
{
}

void VertexNormalTexCoords::setupAttribPointers()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
        sizeof(VertexNormalTexCoords), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
        sizeof(VertexNormalTexCoords), 
        (GLvoid*)offsetof(VertexNormalTexCoords, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        sizeof(VertexNormalTexCoords),
        (GLvoid*)offsetof(VertexNormalTexCoords, texCoords));
}
