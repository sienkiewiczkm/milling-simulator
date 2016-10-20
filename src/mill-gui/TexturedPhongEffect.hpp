#pragma once

#include "Effect.hpp"

#include <glm/glm.hpp>

#include <memory>

class TexturedPhongEffect : 
    public EffectBase
{
public:
    TexturedPhongEffect();
    virtual ~TexturedPhongEffect();

    void create();
    virtual void destroy() override;

    virtual void begin() override;
    virtual void end() override;

    void setTexture(GLuint textureId);

private:
    void createShaders();
};
