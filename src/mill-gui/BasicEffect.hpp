#pragma once
#include "Effect.hpp"

namespace fw
{

class BasicEffect:
    public EffectBase
{
public:
    BasicEffect();
    virtual ~BasicEffect();

    void create();
    virtual void destroy();
    virtual void begin();
    virtual void end();

protected:
    void createShaders();
};

}
