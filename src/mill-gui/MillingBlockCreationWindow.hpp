#pragma once

#include "ImGuiToolboxWindow.hpp"

#include <glm/glm.hpp>

#include <memory>

namespace ms
{
  
class MillingBlock;

class MillingBlockCreationWindow:
    public fw::ImGuiToolboxWindow
{
public:
    MillingBlockCreationWindow();
    virtual ~MillingBlockCreationWindow();

    void open();
    virtual void update() override;
    
    std::shared_ptr<ms::MillingBlock> getNewBlock();

protected:
    void createMillingBlock();

private:
    bool _blockReady;
    glm::vec3 _size;
    glm::ivec2 _resolution;
    float _safeHeight;
};

}
