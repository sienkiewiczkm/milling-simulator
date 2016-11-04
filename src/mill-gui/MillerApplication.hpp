#pragma once

#include "OpenGLApplication.hpp"
#include "OrbitingCamera.hpp"

#include "SimulationModeController.hpp"
#include "DesignModeController.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

enum class MillerAppMode
{
    Simulation,
    Design
};

class MillerApplication :
    public OpenGLApplication
{
public:
    MillerApplication();
    virtual ~MillerApplication();

protected:
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onUpdate();
    virtual void onRender();

    virtual void onMouseButton(int button, int action, int mods);
    virtual void onScroll(double xoffset, double yoffset);
    virtual void onKey(int key, int scancode, int action, int mods);
    virtual void onChar(unsigned int c);

    void handleInput();

protected:
    void updateMainMenuBar();

private:
    unsigned int _frame;
    float _mouseSensitivity;
    glm::vec2 _lastMousePosition;
    bool _showImguiDemo;

    std::shared_ptr<ms::IModeController> _activeMode;
    std::shared_ptr<ms::SimulationModeController> _simulationMode;
    std::shared_ptr<ms::DesignModeController> _designMode;

    OrbitingCamera _camera;
    MillerAppMode _appMode, _previousAppMode;

    double _lastTime;
    double _currentTime;
    double _deltaTime;
};
