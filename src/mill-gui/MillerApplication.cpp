#include "MillerApplication.hpp"
#include "Shaders.hpp"
#include "ImGuiBinding.hpp"
#include "Config.hpp"
#include "TextureUtils.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>

#include <cmath>

using namespace ms;
using namespace std;

MillerApplication::MillerApplication() : 
    _mouseSensitivity(0.05f)
{
}

MillerApplication::~MillerApplication()
{
}

void MillerApplication::onCreate()
{
    ImGuiBinding::initialize(_window, false);

    _texture = loadTextureFromFile(RESOURCE("textures/rustymetal.jpg"));

    const int heightmapWidth = 64;
    const int heightmapHeight = 64;
    vector<float> heightmap = generateHeightmap(
        heightmapWidth, heightmapHeight
    );

    _heightmapGeo.create(heightmapWidth, heightmapHeight);
    _heightmapGeo.setHeightmap(heightmap);

    _lastMousePosition = getMousePosition();

    _cuttingTool.create(0.15f, 0.05f, 0.0f, 0.4f, 0.025f, 0.025f);

    _cuttingToolGUI.setVisibility(true);
    _cuttingToolGUI.setWindowName("Cutting Tool Controller");

    _effect.create();

    _toolController = make_shared<CuttingToolController>();
    _toolController->setMovementSpeed(0.1f);
    _toolController->setStartingPosition(glm::vec3(-0.5f, 0.0f, +0.5f));
    _toolController->setTargetPosition(glm::vec3(+0.5f, 0.0f, -0.5f));
    _toolController->startMovement();

    _cuttingToolGUI.setController(_toolController);
}

void MillerApplication::onDestroy()
{
    ImGuiBinding::shutdown();
}

void MillerApplication::onUpdate()
{
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
    
    const int heightmapWidth = 64;
    const int heightmapHeight = 64;
    vector<float> heightmap = generateHeightmap(
        heightmapWidth, heightmapHeight
    );

    if (!_toolController->isMovementActive())
    {
        _toolController->finishMovement();
        _toolController->setTargetPosition(
            -_toolController->getCurrentPosition()
        );
        _toolController->startMovement();
    }

    _toolController->update(_deltaTime);

    glm::mat4 toolHeightMatrix = glm::translate(glm::mat4(),
        _toolController->getCurrentPosition());

    _cuttingTool.setModelMatrix(toolHeightMatrix);
    _cuttingToolGUI.update();

    _heightmapGeo.setHeightmap(heightmap);
}

void MillerApplication::onRender()
{
    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _effect.begin();
    glm::mat4 model, view, projection;

    float aspectRatio = (float)display_w/display_h;

    view = _camera.getViewMatrix();
    projection = glm::perspective(45.0f, aspectRatio, 0.1f, 100.0f);

    _effect.setModelMatrix(model);
    _effect.setViewMatrix(view);
    _effect.setProjectionMatrix(projection);
    _effect.setTexture(_texture);

    _heightmapGeo.render();

    _cuttingTool.render(&_effect);

    _effect.end();

    ImGui::Render();
}

void MillerApplication::onMouseButton(int button, int action, int mods)
{
    ImGuiBinding::mouseButtonCallback(_window, button, action, mods);
}

void MillerApplication::onScroll(double xoffset, double yoffset)
{
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
        float yfactor = sinf(y*0.2f+phaseShift*0.8f);
        for (int x = 0; x < width; ++x)
        {
            float xfactor = sinf(x*0.1f+phaseShift*1.6f);
            float factor = 0.1f *((xfactor+yfactor) / 2.0f + 1.0f);
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
