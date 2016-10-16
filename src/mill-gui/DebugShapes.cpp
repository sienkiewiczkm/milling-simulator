#include "DebugShapes.hpp"
#include "Common.hpp"

using namespace std;
using namespace glm;

vector<GLfloat> createCube(float width, float height, float length)
{
    float halfWidth = 0.5f * width;
    float halfHeight = 0.5f * height;
    float halfLength = 0.5f * length;

    return {
		-halfWidth, -halfHeight, -halfLength,  0.0f, 0.0f,
		 halfWidth, -halfHeight, -halfLength,  1.0f, 0.0f,
		 halfWidth,  halfHeight, -halfLength,  1.0f, 1.0f,
		 halfWidth,  halfHeight, -halfLength,  1.0f, 1.0f,
		-halfWidth,  halfHeight, -halfLength,  0.0f, 1.0f,
		-halfWidth, -halfHeight, -halfLength,  0.0f, 0.0f,

		-halfWidth, -halfHeight,  halfLength,  0.0f, 0.0f,
		 halfWidth, -halfHeight,  halfLength,  1.0f, 0.0f,
		 halfWidth,  halfHeight,  halfLength,  1.0f, 1.0f,
		 halfWidth,  halfHeight,  halfLength,  1.0f, 1.0f,
		-halfWidth,  halfHeight,  halfLength,  0.0f, 1.0f,
		-halfWidth, -halfHeight,  halfLength,  0.0f, 0.0f,

		-halfWidth,  halfHeight,  halfLength,  1.0f, 0.0f,
		-halfWidth,  halfHeight, -halfLength,  1.0f, 1.0f,
		-halfWidth, -halfHeight, -halfLength,  0.0f, 1.0f,
		-halfWidth, -halfHeight, -halfLength,  0.0f, 1.0f,
		-halfWidth, -halfHeight,  halfLength,  0.0f, 0.0f,
		-halfWidth,  halfHeight,  halfLength,  1.0f, 0.0f,

		 halfWidth,  halfHeight,  halfLength,  1.0f, 0.0f,
		 halfWidth,  halfHeight, -halfLength,  1.0f, 1.0f,
		 halfWidth, -halfHeight, -halfLength,  0.0f, 1.0f,
		 halfWidth, -halfHeight, -halfLength,  0.0f, 1.0f,
		 halfWidth, -halfHeight,  halfLength,  0.0f, 0.0f,
		 halfWidth,  halfHeight,  halfLength,  1.0f, 0.0f,

		-halfWidth, -halfHeight, -halfLength,  0.0f, 1.0f,
		 halfWidth, -halfHeight, -halfLength,  1.0f, 1.0f,
		 halfWidth, -halfHeight,  halfLength,  1.0f, 0.0f,
		 halfWidth, -halfHeight,  halfLength,  1.0f, 0.0f,
		-halfWidth, -halfHeight,  halfLength,  0.0f, 0.0f,
		-halfWidth, -halfHeight, -halfLength,  0.0f, 1.0f,

		-halfWidth,  halfHeight, -halfLength,  0.0f, 1.0f,
		 halfWidth,  halfHeight, -halfLength,  1.0f, 1.0f,
		 halfWidth,  halfHeight,  halfLength,  1.0f, 0.0f,
		 halfWidth,  halfHeight,  halfLength,  1.0f, 0.0f,
		-halfWidth,  halfHeight,  halfLength,  0.0f, 0.0f,
		-halfWidth,  halfHeight, -halfLength,  0.0f, 1.0f
	};
}

Mesh<VertexNormalTexCoords> createCylinder(
    float height, float radius, float circleSubdivisions
)
{
    vector<VertexNormalTexCoords> vertices;
    vector<GLuint> indices;

    for (int angleStep = 0; angleStep <= circleSubdivisions; ++angleStep)
    {
        float progress = angleStep/static_cast<float>(circleSubdivisions);
        float angle = static_cast<float>(2.0*common::pi()*progress);

        float fx = sinf(angle) * radius;
        float fz = cosf(angle) * radius;

        for (int y = -1; y <= 1; y += 2)
        {
            float fy = (0.5f*height) * y;

            vec3 position = vec3(fx, fy, fz);
            vec3 normal = normalize(vec3(fx, 0.0f, fz));
            vec2 texCoord = vec2(progress, 1.0f-(y+1.0f)*0.5f);

            vertices.push_back(VertexNormalTexCoords(
                position,
                normal,
                texCoord
            ));
        }
    }

    int offset = 0;
    for (int angleStep = 0; angleStep < circleSubdivisions; ++angleStep)
    {
        indices.push_back(offset+0);
        indices.push_back(offset+2);
        indices.push_back(offset+1);

        indices.push_back(offset+1);
        indices.push_back(offset+2);
        indices.push_back(offset+3);

        offset += 2;
    }

    return Mesh<VertexNormalTexCoords>(vertices, indices);
}

Mesh<VertexNormalTexCoords> createSphere(
    float radius,
    float latitudeSubdivisions,
    float longtitudeSubdivisions
)
{
    vector<VertexNormalTexCoords> vertices;
    vector<GLuint> indices;
    return Mesh<VertexNormalTexCoords>(vertices, indices);
}
