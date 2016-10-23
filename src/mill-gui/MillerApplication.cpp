#include "MillerApplication.hpp"
#include "Shaders.hpp"
#include "ImGuiBinding.hpp"
#include "Config.hpp"
#include "TextureUtils.hpp"
#include "MillPathFormatReader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <cmath>

using namespace fw;
using namespace ms;
using namespace std;

MillerApplication::MillerApplication() : 
    _frame(0),
    _newBlockRequested(false),
    _mouseSensitivity(0.05f),
    _heightmapResolutionX(32),
    _heightmapResolutionY(32),
    _showImguiDemo(false),
    _showProgramManager(false)
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

    _effect.create();
    _texture = loadTextureFromFile(RESOURCE("textures/rustymetal.jpg"));

    _block = make_shared<MillingBlock>();
    _block->setTexture(_texture);

    _cuttingTool.create(10.0f, 10.0f, 0.0f, 10.0f, 3.5f, 3.5f);
    _toolController = make_shared<CuttingToolController>();
    _toolController->setMovementSpeed(25.0f);
    _toolController->setStartingPosition(_block->getSafePosition());

    _programExecutor = make_shared<MillingProgramExecutor>(
        _toolController
    );

    _programExecutor->setMillingBlock(_block);

    _programExecutorGUI = make_shared<MillingProgramExecutorGUI>(
        _programExecutor
    );

    _programManagerGUI = make_shared<ProgramManagerGUI>(
        _programExecutor
    );

    _createBlockGUI = make_shared<MillingBlockCreationWindow>();

    _cuttingToolGUI.setController(_toolController);
    _cuttingToolGUI.setVisibility(_showProgramManager);
    _cuttingToolGUI.setWindowName("Cutting tool controller");

    _programManagerGUI->setVisibility(_showProgramManager);
}

void MillerApplication::onDestroy()
{
    ImGuiBinding::shutdown();
}

void MillerApplication::onUpdate()
{
    ++_frame;

    ImGuiBinding::newFrame();
    handleInput();

    /* todo: move to framework */
    _lastTime = _currentTime;
    _currentTime = glfwGetTime();
    _deltaTime = _currentTime - _lastTime;

    updateMainMenuBar();

    if (_newBlockRequested)
    {
        _newBlockRequested = false;
        _createBlockGUI->open();
    }

    _createBlockGUI->update();

    auto newBlock = _createBlockGUI->getNewBlock();
    if (newBlock != nullptr)
    {
        _block = newBlock;
        _block->setTexture(_texture);
        _programExecutor->setMillingBlock(_block);
        _toolController->setStartingPosition(_block->getSafePosition());
    }

    static float speed = 1.0f;

    if (_showImguiDemo) {
        ImGui::ShowTestWindow();
    }

    _programManagerGUI->update();
    _programExecutorGUI->update();

    auto errorState = _programExecutor->update(_deltaTime);
    if (errorState != MillingError::None)
    {
        _lastErrorState = errorState;
        ImGui::OpenPopup("Milling error");
    }

    if (ImGui::BeginPopupModal(
            "Milling error",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize
        ))
    {
        std::string errorText = "Unknown error.";
        switch (_lastErrorState)
        {
        case MillingError::None:
            errorText = "No error. Weird.";
            break;
        case MillingError::SafeZoneReached:
            errorText = "Tool has reached the safe zone. Check your program.";
            break;
        case MillingError::DrillingHolesWithFlatTool:
            errorText = "Cannot drill holes using flat tool.";
            break;
        }

        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
            errorText.c_str()
        );

        if (ImGui::Button("OK")) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }

    glm::mat4 toolHeightMatrix = glm::translate(glm::dmat4(),
        _toolController->getCurrentPosition());

    _cuttingTool.setModelMatrix(toolHeightMatrix);
    _cuttingToolGUI.update();

    _block->update();

    _cuttingTool.ensureCompability(_toolController->getCuttingToolParams());
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

    glm::mat4 model, view, projection;
    float aspectRatio = (float)display_w/display_h;

    model = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
    view = _camera.getViewMatrix();
    projection = glm::perspective(45.0f, aspectRatio, 5.0f, 700.0f);

    _effect.begin();
    _effect.setModelMatrix(model);
    _effect.setViewMatrix(view);
    _effect.setProjectionMatrix(projection);
    _effect.setTexture(_texture);
    _cuttingTool.render(&_effect);
    _effect.end();

    _block->setViewMatrix(view);
    _block->setProjectionMatrix(projection);
    _block->render();

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
}

void MillerApplication::onChar(unsigned int c)
{
    ImGuiBinding::charCallback(_window, c);
}

void MillerApplication::handleInput()
{
    ImGuiIO &io = ImGui::GetIO();

    auto mousePosition = getMousePosition();
    auto movement = (mousePosition - _lastMousePosition)*_mouseSensitivity;
    _lastMousePosition = mousePosition;

    if (!io.WantCaptureMouse && 
        GLFW_PRESS == glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT))
    {
        _camera.rotate(movement.y, movement.x);
    }
}

void MillerApplication::updateMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("New"))
            {
                _newBlockRequested = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem(
                "Program manager",
                nullptr,
                _programManagerGUI->getVisibilityFlagPointer()
            );

            ImGui::MenuItem(
                "ImGui Test Window",
                nullptr,
                &_showImguiDemo
            );

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
