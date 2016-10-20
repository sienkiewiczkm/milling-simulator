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

Vertex3D2TexCoord::Vertex3D2TexCoord()
{
}

Vertex3D2TexCoord::Vertex3D2TexCoord(
    glm::vec3 position,
    glm::vec2 texCoord1,
    glm::vec2 texCoord2
) :
    position(position),
    texCoord1(texCoord1),
    texCoord2(texCoord2)
{
}

void Vertex3D2TexCoord::setupAttribPointers()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
        sizeof(Vertex3D2TexCoord),
        reinterpret_cast<GLvoid*>(0)
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 
        sizeof(Vertex3D2TexCoord), 
        reinterpret_cast<GLvoid*>(offsetof(Vertex3D2TexCoord, texCoord1))
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
        sizeof(Vertex3D2TexCoord),
        reinterpret_cast<GLvoid*>(offsetof(Vertex3D2TexCoord, texCoord2))
    );
}
