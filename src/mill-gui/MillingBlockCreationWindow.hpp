#pragma once

#include "ImGuiToolboxWindow.hpp"

#include <glm/glm.hpp>

namespace ms
{
  
class MillingBlockCreationWindow:
    public fw::ImGuiToolboxWindow
{
public:
    MillingBlockCreationWindow();
    virtual ~MillingBlockCreationWindow();

    void open();
    virtual void update() override;

protected:
    void createMillingBlock();

private:
    glm::vec3 _size;
    glm::ivec2 _resolution;
    float _safeHeight;
};

}
