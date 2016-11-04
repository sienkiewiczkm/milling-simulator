#pragma once

#include "OpenGLHeaders.hpp"
#include "BasicEffect.hpp"
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
    void createMeshes();

    GLuint _texture;
    TexturedPhongEffect _effect;
    std::shared_ptr<fw::BasicEffect> _basicEffect;
    std::vector<std::shared_ptr<fw::IParametricSurfaceUV>> _loadedObjects;
    std::vector<std::shared_ptr<Mesh<VertexNormalTexCoords>>>
        _loadedObjectMeshes;

    glm::vec3 _blockSize;
    float _baseHeight;

    std::shared_ptr<Mesh<VertexNormalTexCoords>> _baseBox;
    std::shared_ptr<Mesh<VertexNormalTexCoords>> _blockBoxLimits;
    glm::mat4 _baseBoxModelMatrix, _blockBoxLimitsModelMatrix;

    ImGuizmo::OPERATION _activeOperation;
    glm::mat4 _loadedModelMatrix;
};

}
