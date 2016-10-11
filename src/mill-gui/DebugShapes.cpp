#include "DebugShapes.hpp"

using namespace std;

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
