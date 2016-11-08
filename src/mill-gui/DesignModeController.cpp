#include "DesignModeController.hpp"
#include "DebugShapes.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace ms
{

DesignModeController::DesignModeController():
    _activeOperation{ImGuizmo::TRANSLATE},
    _blockSize{150.0f, 50.0f, 150.0f},
    _baseHeight{21.5f}
{
}

DesignModeController::~DesignModeController()
{
}

void DesignModeController::setWindow(GLFWwindow *window)
{
    _window = window;
}

void DesignModeController::onCreate()
{
    _effect.create();
    _texture = loadTextureFromFile(RESOURCE("textures/opengameart-metal.jpg"));

    _basicEffect = make_shared<fw::BasicEffect>();
    _basicEffect->create();

    CadioModelLoader loader;
    _loadedObjects = loader.loadModel(RESOURCE("models/sienkiewiczk.model"));

    _loadedModelMatrix = glm::translate(
        _loadedModelMatrix,
        glm::vec3{0, 21.35f, 0}
    );

    _loadedModelMatrix = glm::rotate(
        _loadedModelMatrix,
        glm::radians(90.0f),
        glm::vec3{1.0f, 0, 0}
    );

    _loadedModelMatrix = glm::scale(
        _loadedModelMatrix,
        glm::vec3(23.06f, 23.06f, 23.06f)
    );

    fw::ParametricSurfaceMeshBuilder parametricBuilder;
    parametricBuilder.setSamplingResolution(glm::ivec2(64, 64));

    _roughPathGenerator = std::make_shared<RoughMillingPathGenerator>();
    _roughPathGenerator->setCuttingToolRadius(8.0f);
    _roughPathGenerator->setWorkingArea(_blockSize, _baseBoxModelMatrix);
    _roughPathGenerator->setWorkingAreaResolution({300, 300});//0.5mm

    for (const auto &object: _loadedObjects)
    {
        _loadedObjectMeshes.push_back(
            parametricBuilder.build(object)
        );

        _roughPathGenerator->addParametricSurface(
            object,
            _loadedModelMatrix
        );
    }

    createMeshes();
}

void DesignModeController::onActivate()
{
}

void DesignModeController::onDeactivate()
{
}

void DesignModeController::onDestroy()
{
}

void DesignModeController::onUpdate(float deltaTime)
{
    if (ImGui::Begin("Baking process"))
    {
        if (ImGui::Button("Rough milling process"))
        {
            _roughPathGenerator->bake();
            auto program = _roughPathGenerator->buildPaths();

            executor->setProgram("Local program", program);
            CuttingToolParams defaultParameters;
            defaultParameters.kind = CuttingToolKind::Ball;
            defaultParameters.radius = 8.0f;
            executor->getController()->setCuttingToolParams(defaultParameters);
        }
    }
    ImGui::End();
}

void DesignModeController::onRender(const OrbitingCamera &orbitingCamera)
{
    // todo: should be glfw agnostic
    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);

    glm::mat4 model, view, projection;
    float aspectRatio = (float)display_w/display_h;

    model = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
    view = orbitingCamera.getViewMatrix();
    projection = glm::perspective(45.0f, aspectRatio, 5.0f, 700.0f);

    /*
    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
        _activeOperation, ImGuizmo::LOCAL,
        glm::value_ptr(_loadedModelMatrix),
        nullptr,
        nullptr
    );
    */

    _effect.begin();
    _effect.setViewMatrix(view);
    _effect.setProjectionMatrix(projection);
    _effect.setTexture(_texture);

    _effect.setModelMatrix(_baseBoxModelMatrix);
    _baseBox->render();

    _effect.setModelMatrix(_loadedModelMatrix);
    for (const auto &mesh: _loadedObjectMeshes)
    {
        mesh->render();
    }
    _effect.end();

    _basicEffect->begin();
    _basicEffect->setModelMatrix(_blockBoxLimitsModelMatrix);
    _basicEffect->setViewMatrix(view);
    _basicEffect->setProjectionMatrix(projection);
    _blockBoxLimits->render();
    _basicEffect->end();
}

void DesignModeController::updateMainMenuBar()
{
}

void DesignModeController::onKey(int key, int scancode, int action, int mods)
{
    switch (key)
    {
    case GLFW_KEY_Q:
        break;
    case GLFW_KEY_W:
        _activeOperation = ImGuizmo::TRANSLATE;
        break;
    case GLFW_KEY_E:
        _activeOperation = ImGuizmo::ROTATE;
        break;
    case GLFW_KEY_R:
        _activeOperation = ImGuizmo::SCALE;
        break;
    }
}

void DesignModeController::createMeshes()
{
    _basicEffect = make_shared<fw::BasicEffect>();
    _basicEffect->create();

    _baseBox = createBox(glm::vec3{_blockSize.x, _baseHeight, _blockSize.z});
    _baseBoxModelMatrix = glm::translate(
        glm::mat4(),
        glm::vec3{0, _baseHeight/2, 0}
    );

    glm::vec3 deltaSize(0.001f, 0.001f, 0.001f);
    _blockBoxLimits = createBox(_blockSize - deltaSize);
    _blockBoxLimitsModelMatrix = glm::translate(
        glm::mat4(),
        glm::vec3{0, _blockSize.y/2, 0}
    );
}

}