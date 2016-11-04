#pragma once

#include "OrbitingCamera.hpp"

namespace ms
{

class IModeController
{
public:
    virtual ~IModeController() = default;

    virtual void onActivate() = 0;
    virtual void onDeactivate() = 0;
    virtual void onCreate() = 0;
    virtual void onDestroy() = 0;
    virtual void onUpdate(float deltaTime) = 0;
    virtual void onRender(const OrbitingCamera &orbitingCamera) = 0;
    virtual void onKey(int key, int scancode, int action, int mods) = 0;
    virtual void updateMainMenuBar() = 0;
};

}
