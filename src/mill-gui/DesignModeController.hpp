#pragma once

#include "OpenGLHeaders.hpp"
#include "OrbitingCamera.hpp"
#include "Config.hpp"
#include "TextureUtils.hpp"
#include "Shaders.hpp"
#include "TexturedPhongEffect.hpp"
#include "CadioModelLoader.hpp"
#include "SimulationModeController.hpp"
#include "BsplineEquidistantKnotGenerator.hpp"
#include "ParametricSurfaceMeshBuilder.hpp"
#include "BsplineNonVanishingReparametrization.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

namespace ms
{

class DesignModeController:
    public IModeController
{
public:
    DesignModeController();
    virtual ~DesignModeController();

    // todo: remove this
    void setWindow(GLFWwindow *window);

    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onUpdate(float deltaTime);
    virtual void onRender(const OrbitingCamera &orbitingCamera);
    virtual void updateMainMenuBar();
    virtual void onKey(int key, int scancode, int action, int mods);

protected:
    GLFWwindow *_window;

private:
    GLuint _texture;
    TexturedPhongEffect _effect;
    std::vector<std::shared_ptr<fw::IParametricSurfaceUV>> _loadedObjects;
    std::vector<std::shared_ptr<Mesh<VertexNormalTexCoords>>>
        _loadedObjectMeshes;

    ImGuizmo::OPERATION _activeOperation;
    glm::mat4 _loadedModelMatrix;
};

}
