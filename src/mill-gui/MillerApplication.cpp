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
    _camera.setDist(100.0f);

    ImGuiBinding::initialize(_window, false);

    _texture = loadTextureFromFile(RESOURCE("textures/rustymetal.jpg"));

    vector<float> heightmap = generateHeightmap(
        _heightmapResolutionX, _heightmapResolutionY
    );

    _heightmapGeo.create(_heightmapResolutionX, _heightmapResolutionY, 
            glm::vec3(1.0f, 1.0f, 1.0f));
    _heightmapGeo.setHeightmap(heightmap);

    _lastMousePosition = getMousePosition();

    _cuttingTool.create(50.0f, 20.0f, 0.0f, 20.0f, 7.0f, 7.0f);
    _effect.create();

    _heightmapEffect = make_shared<ms::HeightmapVisualizationEffect>();
    _heightmapEffect->create();

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

    vector<float> heightmap = generateHeightmap(
        _heightmapResolutionX, _heightmapResolutionY
    );

    _heightmapGeo.setHeightmap(heightmap);
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

    model = glm::scale(glm::mat4(), glm::vec3(100.0f, 100.0f, 100.0f));
    view = _camera.getViewMatrix();
    projection = glm::perspective(45.0f, aspectRatio, 5.0f, 700.0f);

    _effect.begin();
    _effect.setModelMatrix(model);
    _effect.setViewMatrix(view);
    _effect.setProjectionMatrix(projection);
    _effect.setTexture(_texture);
    _cuttingTool.render(&_effect);
    _effect.end();

    _heightmapEffect->begin();
    _heightmapEffect->setModelMatrix(model);
    _heightmapEffect->setViewMatrix(view);
    _heightmapEffect->setProjectionMatrix(projection);
    _heightmapEffect->setAlbedoTexture(_texture);
    _heightmapGeo.render();
    _heightmapEffect->end(); 

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
