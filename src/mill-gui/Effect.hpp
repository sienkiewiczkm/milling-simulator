#pragma once

#include "Shaders.hpp"

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace uniforms
{
    extern const std::string ModelMatrixName;
    extern const std::string ViewMatrixName;
    extern const std::string ProjectionMatrixName;
}

class IEffect
{
public:
    virtual ~IEffect();

    virtual void destroy() = 0;

    virtual void begin() = 0;
    virtual void end() = 0;

    virtual glm::mat4 getModelMatrix() = 0;
    virtual void setModelMatrix(const glm::mat4 &modelMatrix) = 0;

    virtual glm::mat4 getViewMatrix() = 0;
    virtual void setViewMatrix(const glm::mat4 &viewMatrix) = 0;

    virtual glm::mat4 getProjectionMatrix() = 0;
    virtual void setProjectionMatrix(const glm::mat4 &projMatrix) = 0;
};

class EffectBase : public IEffect
{
public:
    EffectBase();
    virtual ~EffectBase();

    virtual void destroy() = 0;

    virtual void begin() = 0;
    virtual void end() = 0;

    virtual glm::mat4 getModelMatrix();
    virtual void setModelMatrix(const glm::mat4 &modelMatrix);

    virtual glm::mat4 getViewMatrix();
    virtual void setViewMatrix(const glm::mat4 &viewMatrix);

    virtual glm::mat4 getProjectionMatrix();
    virtual void setProjectionMatrix(const glm::mat4 &projMatrix);

protected:
    std::shared_ptr<ShaderProgram> _shaderProgram;    
    glm::mat4 _modelMatrix, _viewMatrix, _projMatrix;
};
