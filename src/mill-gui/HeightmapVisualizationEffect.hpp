#pragma once

#include "Effect.hpp"

#include <glm/glm.hpp>

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

    void setTextureMatrix(const glm::mat4 &textureMatrix);

    void setAlbedoTexture(GLuint textureId);
    void setHeightmapTexture(GLuint textureId);
    void setSize(const glm::vec3 &size);

protected:
    void createShaders();

private:
    glm::mat4 _textureMatrix;
};

}
