#include "MillerApplication.hpp"
#include "Shaders.hpp"
#include "ImGuiBinding.hpp"
#include "Config.hpp"

#include "SimulationModeController.hpp"
#include "DesignModeController.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

#include <cmath>

using namespace fw;
using namespace ms;
using namespace std;

MillerApplication::MillerApplication() :
    _frame(0),
    _mouseSensitivity(0.05f),
    _showImguiDemo(false)
{
}

MillerApplication::~MillerApplication()
{
}

void MillerApplication::onCreate()
{
    ImGuiBinding::initialize(_window, false);
    _lastMousePosition = getMousePosition();
    _camera.setDist(100.0f);

    _simulationMode = std::make_shared<SimulationModeController>();
    _simulationMode->setWindow(_window);
    _simulationMode->onCreate();
    _simulationMode->onActivate();

    _designMode = std::make_shared<DesignModeController>();
    _designMode->setWindow(_window);
    _designMode->onCreate();

    _activeMode = std::static_pointer_cast<ms::IModeController>(
        _simulationMode
    );
}

void MillerApplication::onDestroy()
{
    ImGuiBinding::shutdown();
}

void MillerApplication::onUpdate()
{
    _previousAppMode = _appMode;
    ++_frame;

    ImGuiBinding::newFrame();
    ImGuizmo::BeginFrame();
    handleInput();

    /* todo: move to framework */
    _lastTime = _currentTime;
    _currentTime = glfwGetTime();
    _deltaTime = _currentTime - _lastTime;

    updateMainMenuBar();

    if (_appMode != _previousAppMode)
    {
        _activeMode->onDeactivate();

        switch (_appMode)
        {
        case MillerAppMode::Simulation:
            _activeMode = std::static_pointer_cast<ms::IModeController>(
                _simulationMode
            );
            break;
        case MillerAppMode::Design:
            _activeMode = std::static_pointer_cast<ms::IModeController>(
                _designMode
            );
            break;
        }
    }

    _activeMode->onUpdate(_deltaTime);

    if (_showImguiDemo) {
        ImGui::ShowTestWindow();
    }
}

void MillerApplication::onRender()
{
    /* todo: move to framework */
    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);
    glClearColor(0.25f, 0.25f, 0.25f, 1.0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _activeMode->onRender(_camera);

    ImGui::Render();
}

void MillerApplication::onMouseButton(int button, int action, int mods)
{
    ImGuiBinding::mouseButtonCallback(_window, button, action, mods);
}

void MillerApplication::onScroll(double xoffset, double yoffset)
{
    float zoomDifference = yoffset * 0.5f;
    _camera.setDist(
        max(_camera.getDist() + zoomDifference, 10.0f)
    );
    ImGuiBinding::scrollCallback(_window, xoffset, yoffset);
}

void MillerApplication::onKey(int key, int scancode, int action, int mods)
{
    ImGuiBinding::keyCallback(_window, key, scancode, action, mods);

    ImGuiIO &io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;

    _activeMode->onKey(key, scancode, action, mods);
}

void MillerApplication::onChar(unsigned int c)
{
    ImGuiBinding::charCallback(_window, c);
}

void MillerApplication::handleInput()
{
    ImGuiIO &io = ImGui::GetIO();

    /* todo: move to framework */
    auto mousePosition = getMousePosition();
    auto movement = (mousePosition - _lastMousePosition)*_mouseSensitivity;
    _lastMousePosition = mousePosition;

    if (!io.WantCaptureMouse && !ImGuizmo::IsUsing() &&
        GLFW_PRESS == glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT))
    {
        _camera.rotate(movement.y, movement.x);
    }
}

void MillerApplication::updateMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Mode"))
        {
            bool isInSimulationMode = _appMode == MillerAppMode::Simulation;
            bool isInDesignMode = _appMode == MillerAppMode::Design;

            if (ImGui::MenuItem("Simulation", nullptr, &isInSimulationMode))
            {
                _appMode = MillerAppMode::Simulation;
            }

            if (ImGui::MenuItem("Design", nullptr, &isInDesignMode))
            {
                _appMode = MillerAppMode::Design;
            }

            ImGui::EndMenu();
        }

        _activeMode->updateMainMenuBar();

        ImGui::EndMainMenuBar();
    }
}


