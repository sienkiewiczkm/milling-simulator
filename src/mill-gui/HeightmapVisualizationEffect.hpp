#pragma once

#include "Effect.hpp"

#include <memory>

namespace ms
{

class HeightmapVisualizationEffect:
    public EffectBase
{
public:
    HeightmapVisualizationEffect();
    virtual ~HeightmapVisualizationEffect();

    void create();
    void destroy();

    void begin();
    void end();

    void setAlbedoTexture(GLuint textureId);

    void setHeightmapTexture(GLuint textureId);

protected:
    void createShaders();
};

}
