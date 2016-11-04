#include "DesignModeController.hpp"

namespace ms
{

DesignModeController::DesignModeController():
    _activeOperation{ImGuizmo::TRANSLATE}
{
    _loadedModelMatrix = glm::scale(
        glm::mat4(),
        glm::vec3(10.0f, 10.0f, 10.0f)
    );
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

    CadioModelLoader loader;
    _loadedObjects = loader.loadModel(RESOURCE("models/sienkiewiczk.model"));

    fw::ParametricSurfaceMeshBuilder parametricBuilder;
    parametricBuilder.setSamplingResolution(glm::ivec2(16, 16));

    for (const auto &object: _loadedObjects)
    {
        _loadedObjectMeshes.push_back(
            parametricBuilder.build(object)
        );
    }
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

    ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
        _activeOperation, ImGuizmo::LOCAL,
        glm::value_ptr(_loadedModelMatrix),
        nullptr,
        nullptr
    );

    _effect.begin();
    _effect.setModelMatrix(_loadedModelMatrix);
    _effect.setViewMatrix(view);
    _effect.setProjectionMatrix(projection);
    _effect.setTexture(_texture);
    for (const auto &mesh: _loadedObjectMeshes)
    {
        mesh->render();
    }
    _effect.end();
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

}
