#include "TextureUtils.hpp"
#include <stb_image.h>

using namespace std;

GLuint loadTextureFromFile(const string &filename)
{
    int width, height, components;
    unsigned char *image = stbi_load(
        filename.c_str(), &width, &height, &components, 0
    );

    if (image == nullptr)
    {
        throw string("Cannot load specified texture: " + filename);
    }

    GLenum componentsEnum;
    switch (components)
    {
    case 3:
        componentsEnum = GL_RGB;
        break;
    case 4:
        componentsEnum = GL_RGBA;
        break;
    default:
        stbi_image_free(image);
        throw string("Components amount currently not supported.");
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, componentsEnum, width, height, 0,
        componentsEnum, GL_UNSIGNED_BYTE, image);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(image);

    return texture;
}
