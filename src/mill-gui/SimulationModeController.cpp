#include "SimulationModeController.hpp"

using namespace fw;
using namespace std;

namespace ms
{

SimulationModeController::SimulationModeController():
    _newBlockRequested(false),
    _heightmapResolutionX(32),
    _heightmapResolutionY(32),
    _showProgramManager(false),
    _lastErrorState(MillingError::None)
{
}

SimulationModeController::~SimulationModeController()
{
}

void SimulationModeController::setWindow(GLFWwindow *window)
{
    _window = window;
}

void SimulationModeController::onCreate()
{
    _effect.create();
    _texture = loadTextureFromFile(RESOURCE("textures/opengameart-metal.jpg"));

    _block = make_shared<MillingBlock>();
    _block->setTexture(_texture);

    _cuttingTool.create(10.0f, 10.0f, 0.0f, 30.0f, 3.5f, 3.5f);
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

void SimulationModeController::onActivate()
{
}

void SimulationModeController::onDeactivate()
{
}

void SimulationModeController::onDestroy()
{
}

void SimulationModeController::onUpdate(float deltaTime)
{
    if (_newBlockRequested)
    {
        _newBlockRequested = false;
        _createBlockGUI->open();
    }

    if (!isGUIHidden())
    {
        _createBlockGUI->update();
        _cuttingToolGUI.update();
        _programManagerGUI->update();
    }

    _programExecutorGUI->update();

    auto newBlock = _createBlockGUI->getNewBlock();
    if (newBlock != nullptr)
    {
        _block = newBlock;
        _block->setTexture(_texture);
        _programExecutor->setMillingBlock(_block);
        _toolController->setStartingPosition(_block->getSafePosition());
    }

    auto errorState = _programExecutor->update(deltaTime);
    if (errorState != MillingError::None)
    {
        _lastErrorState = errorState;
    }

    if ( _lastErrorState != MillingError::None)
    {
        ImGui::OpenPopup("Milling error");
    }

    updateMillingErrorPopup();
    updateRenderables();
}

void SimulationModeController::onRender(const OrbitingCamera &orbitingCamera)
{
    if (isGeometryRenderingEnabled())
    {
        renderSceneGeometry(orbitingCamera);
    }
}

void SimulationModeController::updateMainMenuBar()
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

        ImGui::EndMenu();
    }
}

bool SimulationModeController::isGUIHidden()
{
    return _programExecutor->isInFastForwardMode();
}

void SimulationModeController::updateMillingErrorPopup()
{
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
        case MillingError::FastMovementCollision:
            errorText = "Tool cannot touch the material during fast move.";
            break;
        }

        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
            errorText.c_str()
        );

        if (ImGui::Button("OK"))
        {
            ImGui::CloseCurrentPopup();
            _lastErrorState = MillingError::None;
        }

        ImGui::EndPopup();
    }
}

void SimulationModeController::updateRenderables()
{
    updateCuttingTool();
    _block->update();
}

void SimulationModeController::updateCuttingTool()
{
    glm::mat4 toolHeightMatrix = glm::translate(
        glm::dmat4(),
        _toolController->getCurrentPosition()
    );

    _cuttingTool.ensureCompability(_toolController->getCuttingToolParams());
    _cuttingTool.setModelMatrix(toolHeightMatrix);
}

bool SimulationModeController::isGeometryRenderingEnabled()
{
    return !(_programExecutor->isInFastForwardMode());
}

void SimulationModeController::renderSceneGeometry(
    const OrbitingCamera &orbitingCamera
)
{
    // todo: should be glfw agnostic
    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);

    glm::mat4 model, view, projection;
    float aspectRatio = (float)display_w/display_h;

    model = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
    view = orbitingCamera.getViewMatrix();
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
}

void SimulationModeController::onKey(int key, int scancode, int action, int mods)
{
}

std::shared_ptr<ms::MillingProgramExecutor>
    SimulationModeController::getProgramExecutor()
{
    return _programExecutor;
}

}
