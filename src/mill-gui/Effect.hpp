#pragma once

#include <glm/glm.hpp>

class IEffect
{
public:
    IEffect();
    virtual ~IEffect();

    virtual void destroy() = 0;

    virtual void begin() = 0;
    virtual void end() = 0;

    virtual void setModelMatrix(const glm::mat4 &modelMatrix) = 0;
    virtual void setViewMatrix(const glm::mat4 &viewMatrix) = 0;
    virtual void setProjectionMatrix(const glm::mat4 &projMatrix) = 0;
};
