#include "Effect.hpp"

#include <glm/gtc/type_ptr.hpp>

namespace uniforms
{
    const std::string ModelMatrixName = "model";
    const std::string ViewMatrixName = "view";
    const std::string ProjectionMatrixName = "projection";
}

IEffect::~IEffect()
{
}

EffectBase::EffectBase()
{
}

EffectBase::~EffectBase()
{
}

glm::mat4 EffectBase::getModelMatrix()
{
    return _modelMatrix;
}

void EffectBase::setModelMatrix(const glm::mat4 &modelMatrix)
{
    _modelMatrix = modelMatrix;

    GLint uniformLoc = glGetUniformLocation(
        _shaderProgram->getId(),
        uniforms::ModelMatrixName.c_str()
    );

    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
}

glm::mat4 EffectBase::getViewMatrix()
{
    return _viewMatrix;
}

void EffectBase::setViewMatrix(const glm::mat4 &viewMatrix)
{
    _viewMatrix = viewMatrix;

    GLint uniformLoc = glGetUniformLocation(
        _shaderProgram->getId(),
        uniforms::ViewMatrixName.c_str()
    );

    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

glm::mat4 EffectBase::getProjectionMatrix()
{
    return _projMatrix;
}

void EffectBase::setProjectionMatrix(const glm::mat4 &projMatrix)
{
    _projMatrix = projMatrix;

    GLint uniformLoc = glGetUniformLocation(
        _shaderProgram->getId(),
        uniforms::ProjectionMatrixName.c_str()
    );

    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
}
