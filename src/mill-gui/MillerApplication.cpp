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
    _mouseSensitivity(0.05f),
    _heightmapResolutionX(512),
    _heightmapResolutionY(512)
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

    _texture = loadTextureFromFile(RESOURCE("textures/rustymetal.jpg"));

    _cuttingTool.create(10.0f, 10.0f, 0.0f, 10.0f, 3.5f, 3.5f);
    _effect.create();

    MillPathFormatReader reader;
    reader.readFromFile(RESOURCE("paths/t1.k16"));
    _movements = reader.getMovements();

    _toolController = make_shared<CuttingToolController>();
    _toolController->setMovementSpeed(8.0f);

    if (_movements.size() > 1)
    {
        _toolController->setStartingPosition(_movements[0].position);
        _toolController->setTargetPosition(_movements[1].position);
        _toolController->startMovement();
        _currentMoveIndex = 1;
    }

    _cuttingToolGUI.setController(_toolController);
    _cuttingToolGUI.setVisibility(true);
    _cuttingToolGUI.setWindowName("Cutting Tool Controller");

    _block = make_shared<MillingBlock>();
    _block->setTexture(_texture);
}

void MillerApplication::onDestroy()
{
    ImGuiBinding::shutdown();
}

void MillerApplication::onUpdate()
{
    ++_frame;

    static bool showSimulationTools = false;
    static bool showImguiDemo = false;

    ImGuiBinding::newFrame();
    handleInput();

    /* todo: move to framework */
    _lastTime = _currentTime;
    _currentTime = glfwGetTime();
    _deltaTime = _currentTime - _lastTime;

    bool booleanset = false;
    
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Tools"))
        {
            ImGui::MenuItem("Simulation tools", "CTRL+S", &showSimulationTools);
            ImGui::MenuItem("ImGui Test Window", nullptr, &showImguiDemo);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                booleanset = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    static float speed = 1.0f;

    if (showImguiDemo) {
        ImGui::ShowTestWindow();
    }

    if (showSimulationTools)
    {
        ImGui::Begin("Simulation tools", &showSimulationTools);
        ImGui::Button("Play"); ImGui::SameLine();
        ImGui::Button("Pause"); ImGui::SameLine();
        ImGui::Button("Stop");
        ImGui::SliderFloat("Speed", &speed, 0.0f, 10.0f);

        ImGui::ProgressBar(0.50, ImVec2(0.0f,0.0f));
        ImGui::SameLine(0.0f);
        ImGui::Text("Progress");

        ImGui::End();
    }

    static float f = 0.0f;
    ImGui::Text("Hello world!");
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    if (ImGui::Button("Show this thing"))
    {
        ImGui::BulletText("Okay, I'm showing");
    }
    
    if (!_toolController->isMovementActive() &&
        _currentMoveIndex < _movements.size())
    {
        _currentMoveIndex++;
        _toolController->finishMovement();
        _toolController->setTargetPosition(
            _movements[_currentMoveIndex].position
        );
        _toolController->startMovement();
    }

    _toolController->update(_deltaTime);

    glm::mat4 toolHeightMatrix = glm::translate(glm::dmat4(),
        _toolController->getCurrentPosition());

    _cuttingTool.setModelMatrix(toolHeightMatrix);
    _cuttingToolGUI.update();

    CuttingToolParams params;
    params.radius = 3.5f;
    params.kind = CuttingToolKind::Flat;

    _block->moveTool(
        _toolController->getLastPosition(),
        _toolController->getCurrentPosition(),
        params
    );

    _block->update();
}

void MillerApplication::onRender()
{
    /* todo: move to framework */
    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);

    glViewport(0, 0, display_w, display_h);
    glClearColor(1.0, 0.0, 1.0, 1.0);
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

vector<float> MillerApplication::generateHeightmap(int width, int height)
{
    float phaseShift = glfwGetTime();

    vector<float> heightmap;

    for (int y = 0; y < height; ++y)
    {
        float yfactor = 0.0f;//sinf(0.02*y+phaseShift*0.8f);
        for (int x = 0; x < width; ++x)
        {
            float xfactor = sinf(x*0.01f+phaseShift*1.6f);
            float factor = 50.f * (xfactor+yfactor+2.0f)*0.25f;
            heightmap.push_back(factor);
        }
    }

    return heightmap;
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
