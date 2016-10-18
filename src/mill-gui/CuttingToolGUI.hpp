#pragma once

#include <glm/glm.hpp>

#include <string>
#include <memory>

namespace ms { class CuttingToolController; }

class CuttingToolGUI
{
public:
    CuttingToolGUI();

    void setVisibility(bool isVisible);
    void setWindowName(const std::string &windowName);
    void setController(std::shared_ptr<ms::CuttingToolController> controller);

    void update();

private:
    std::string _windowName;
    bool _isVisible;

    std::shared_ptr<ms::CuttingToolController> _controller;
    glm::vec3 _desiredPosition;
};
