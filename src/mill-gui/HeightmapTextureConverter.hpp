#pragma once

#include "OpenGLHeaders.hpp"

#include <vector>

namespace fw
{

class HeightmapTextureConverter
{
public:
    HeightmapTextureConverter();
    ~HeightmapTextureConverter();;

    GLuint createTextureFromHeightmap(
        const std::vector<float> &heightmap,
        int width, int length
    );

    void updateTextureFromHeightmap(
        GLuint textureId,
        const std::vector<float> &heightmap,
        int width, int length
    );
};

}
