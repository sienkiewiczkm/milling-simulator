#include "DebugShapes.hpp"
#include "Common.hpp"

using namespace std;
using namespace glm;

Mesh<VertexNormalTexCoords> createPlane(float width, float length)
{
    std::vector<VertexNormalTexCoords> vertices;

    float halfWidth = 0.5f * width;
    float halfLength = 0.5f * length;

    for (int i = 0; i < 4; ++i)
    {
        int x = i % 2;
        int z = i / 2;

        int signX = x == 0 ? -1 : 1;
        int signZ = z == 0 ? -1 : 1;

        glm::vec3 position(signX * halfWidth, 0.0f, signZ * halfLength);
        glm::vec3 normal(0.0f, 1.0f, 0.0f);
        glm::vec2 texCoord(static_cast<float>(x), static_cast<float>(z));

        auto vertex = VertexNormalTexCoords(position, normal, texCoord);
        vertices.push_back(vertex);
    }

    std::vector<GLuint> indices = { 0, 1, 2, 1, 2, 3 };

    return Mesh<VertexNormalTexCoords>(vertices, indices);
}

vector<GLfloat> createCube(float width, float height, float length)
{
    float halfWidth = 0.5f * width;
    float halfHeight = 0.5f * height;
    float halfLength = 0.5f * length;

    return {
		-halfWidth, -halfHeight, -halfLength,  0.0f, 0.0f,
		 halfWidth, -halfHeight, -halfLength,  1.0f, 0.0f,
		 halfWidth,  halfHeight, -halfLength,  1.0f, 1.0f,
		 halfWidth,  halfHeight, -halfLength,  1.0f, 1.0f, -halfWidth,  halfHeight, -halfLength,  0.0f, 1.0f,
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

std::shared_ptr<Mesh<VertexNormalTexCoords>> createBox(const glm::vec3 &size)
{
    std::vector<GLuint> indices;

    auto halfWidth = size.x/2;
    auto halfHeight = size.y/2;
    auto halfLength = size.z/2;

    auto xAxis = glm::vec3(1.0, 0.0, 0.0);
    auto yAxis = glm::vec3(0.0, 1.0, 0.0);
    auto zAxis = glm::vec3(0.0, 0.0, 1.0);

    std::vector<VertexNormalTexCoords> vertices
    {
        {{-halfWidth, -halfHeight, -halfLength}, -zAxis, {0.0f, 0.0f}},
        {{ halfWidth, -halfHeight, -halfLength}, -zAxis, {1.0f, 0.0f}},
        {{ halfWidth,  halfHeight, -halfLength}, -zAxis, {1.0f, 1.0f}},
        {{ halfWidth,  halfHeight, -halfLength}, -zAxis, {1.0f, 1.0f}},
        {{-halfWidth,  halfHeight, -halfLength}, -zAxis, {0.0f, 1.0f}},
        {{-halfWidth, -halfHeight, -halfLength}, -zAxis, {0.0f, 0.0f}},

        {{-halfWidth, -halfHeight,  halfLength}, +zAxis, {0.0f, 0.0f}},
        {{ halfWidth, -halfHeight,  halfLength}, +zAxis, {1.0f, 0.0f}},
        {{ halfWidth,  halfHeight,  halfLength}, +zAxis, {1.0f, 1.0f}},
        {{ halfWidth,  halfHeight,  halfLength}, +zAxis, {1.0f, 1.0f}},
        {{-halfWidth,  halfHeight,  halfLength}, +zAxis, {0.0f, 1.0f}},
        {{-halfWidth, -halfHeight,  halfLength}, +zAxis, {0.0f, 0.0f}},

        {{-halfWidth,  halfHeight,  halfLength}, -xAxis, {1.0f, 0.0f}},
        {{-halfWidth,  halfHeight, -halfLength}, -xAxis, {1.0f, 1.0f}},
        {{-halfWidth, -halfHeight, -halfLength}, -xAxis, {0.0f, 1.0f}},
        {{-halfWidth, -halfHeight, -halfLength}, -xAxis, {0.0f, 1.0f}},
        {{-halfWidth, -halfHeight,  halfLength}, -xAxis, {0.0f, 0.0f}},
        {{-halfWidth,  halfHeight,  halfLength}, -xAxis, {1.0f, 0.0f}},

        {{ halfWidth,  halfHeight,  halfLength}, +xAxis, {1.0f, 0.0f}},
        {{ halfWidth,  halfHeight, -halfLength}, +xAxis, {1.0f, 1.0f}},
        {{ halfWidth, -halfHeight, -halfLength}, +xAxis, {0.0f, 1.0f}},
        {{ halfWidth, -halfHeight, -halfLength}, +xAxis, {0.0f, 1.0f}},
        {{ halfWidth, -halfHeight,  halfLength}, +xAxis, {0.0f, 0.0f}},
        {{ halfWidth,  halfHeight,  halfLength}, +xAxis, {1.0f, 0.0f}},

        {{-halfWidth, -halfHeight, -halfLength}, -yAxis, {0.0f, 1.0f}},
        {{ halfWidth, -halfHeight, -halfLength}, -yAxis, {1.0f, 1.0f}},
        {{ halfWidth, -halfHeight,  halfLength}, -yAxis, {1.0f, 0.0f}},
        {{ halfWidth, -halfHeight,  halfLength}, -yAxis, {1.0f, 0.0f}},
        {{-halfWidth, -halfHeight,  halfLength}, -yAxis, {0.0f, 0.0f}},
        {{-halfWidth, -halfHeight, -halfLength}, -yAxis, {0.0f, 1.0f}},

        {{-halfWidth,  halfHeight, -halfLength}, +yAxis, {0.0f, 1.0f}},
        {{ halfWidth,  halfHeight, -halfLength}, +yAxis, {1.0f, 1.0f}},
        {{ halfWidth,  halfHeight,  halfLength}, +yAxis, {1.0f, 0.0f}},
        {{ halfWidth,  halfHeight,  halfLength}, +yAxis, {1.0f, 0.0f}},
        {{-halfWidth,  halfHeight,  halfLength}, +yAxis, {0.0f, 0.0f}},
        {{-halfWidth,  halfHeight, -halfLength}, +yAxis, {0.0f, 1.0f}}
	};

    for (auto i = 0; i < 6; ++i)
    {
        auto base = 6 * i;
        for (auto j = 0; j < 6; ++j)
        {
            indices.push_back(base+j);
        }
    }

    return std::make_shared<Mesh<VertexNormalTexCoords>>(vertices, indices);
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

    for (int y = -1; y <= 1; y += 2)
    {
        int centralVertexIndex = vertices.size();

        float fy = 0.5f * height * y;
        vec3 normal = vec3(0.0f, fy, 0.0f);

        vertices.push_back(VertexNormalTexCoords(
            vec3(0.0f, fy, 0.0f),
            normal,
            vec2(0.5f, 0.5f)
        ));

        for (int angleStep = 0; angleStep <= circleSubdivisions; ++angleStep)
        {
            float theta = 2.0f * common::pif()
                * angleStep/static_cast<float>(circleSubdivisions);

            float fx = sinf(theta);
            float fz = cosf(theta);

            vertices.push_back(VertexNormalTexCoords(
                vec3(radius * fx, fy, radius * fz),
                normal,
                0.5f * vec2(1.0f + fx, 1.0f + fz)
            ));
        }

        for (int vertexId = 0; vertexId < circleSubdivisions; ++vertexId)
        {
            indices.push_back(centralVertexIndex);
            indices.push_back(centralVertexIndex+1+vertexId);
            indices.push_back(centralVertexIndex+1+vertexId+1);
        }
    }

    return Mesh<VertexNormalTexCoords>(vertices, indices);
}

Mesh<VertexNormalTexCoords> createSphere(
    float radius,
    int latitudeSubdivisions,
    int longtitudeSubdivisions
)
{
    vector<VertexNormalTexCoords> vertices;
    vector<GLuint> indices;

    for (int latitude = 0;
         latitude < latitudeSubdivisions;
         ++latitude)
    {
        for (int longtitude = 0;
             longtitude < longtitudeSubdivisions;
             ++longtitude)
        {
            float theta = 2.0f * common::pif() *
                latitude / static_cast<float>(latitudeSubdivisions);
            float phi = common::pif() *
                longtitude / (static_cast<float>(longtitudeSubdivisions) - 1);

            float costheta = cosf(theta);
            float sintheta = sinf(theta);
            float cosphi = cosf(phi);
            float sinphi = sinf(phi);

            glm::vec3 position = glm::vec3(
                radius * costheta * sinphi,
                radius * sintheta * sinphi,
                radius * cosphi
            );

            vertices.push_back(VertexNormalTexCoords(
                position,
                glm::normalize(position),
                glm::vec2(0.0f, 0.0f)
            ));
        }
    }


    for (int latitude = 0;
         latitude < latitudeSubdivisions;
         ++latitude)
    {
        for (int longtitude = 0;
             longtitude < longtitudeSubdivisions - 1;
             ++longtitude)
        {
            int baseIndex = latitude * longtitudeSubdivisions + longtitude;
            int neighbourIndex = ((latitude + 1) % latitudeSubdivisions)
                * longtitudeSubdivisions + longtitude;

            indices.push_back(baseIndex);
            indices.push_back(baseIndex+1);
            indices.push_back(neighbourIndex);

            indices.push_back(neighbourIndex);
            indices.push_back(baseIndex+1);
            indices.push_back(neighbourIndex+1);
        }
    }

    return Mesh<VertexNormalTexCoords>(vertices, indices);
}
