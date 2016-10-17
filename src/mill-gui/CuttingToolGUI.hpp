#pragma once

#include <glm/glm.hpp>

#include <string>

class CuttingToolGUI
{
public:
    CuttingToolGUI();

    void setVisibility(bool isVisible);
    void setWindowName(const std::string &windowName);

    void update();

private:
    std::string _windowName;
    bool _isVisible;
    glm::vec3 _tipPosition;
    glm::vec3 _desiredPosition;
};
