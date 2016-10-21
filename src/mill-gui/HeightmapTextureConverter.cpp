#include "HeightmapTextureConverter.hpp"

#include <cassert>

namespace fw
{

HeightmapTextureConverter::HeightmapTextureConverter()
{
}

HeightmapTextureConverter::~HeightmapTextureConverter()
{
}

GLuint HeightmapTextureConverter::createTextureFromHeightmap(
    const std::vector<float> &heightmap,
    int width, int length
)
{
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, length, 0,
        GL_RED, GL_FLOAT, heightmap.data());

    return texture;
}

void HeightmapTextureConverter::updateTextureFromHeightmap(
    GLuint textureId,
    const std::vector<float> &heightmap,
    int width, int length
)
{
    glBindTexture(GL_TEXTURE_2D, textureId);

    GLint texWidth, texHeight;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);

    assert(texWidth == width && texHeight == length
        && heightmap.size() == (width * length));

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texWidth, texHeight, GL_RED,
        GL_FLOAT, heightmap.data());
}

}
