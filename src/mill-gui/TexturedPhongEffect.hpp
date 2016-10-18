#pragma once

#include "Config.hpp"
#include "Effect.hpp"
#include "Shaders.hpp"

#include <glm/glm.hpp>

#include <memory>

class TexturedPhongEffect : 
    public IEffect
{
public:
    TexturedPhongEffect();
    virtual ~TexturedPhongEffect();

    void create();
    virtual void destroy() override;

    virtual void begin() override;
    virtual void end() override;

    virtual void setModelMatrix(const glm::mat4 &modelMatrix) override;
    virtual void setViewMatrix(const glm::mat4 &viewMatrix) override;
    virtual void setProjectionMatrix(const glm::mat4 &projMatrix) override;

    void setTexture(GLuint textureId);

private:
    void createShaders();

    std::shared_ptr<ShaderProgram> _program;
    glm::mat4 _modelMatrix, _viewMatrix, _projMatrix;
};
