#pragma once

#include "BasicEffect.hpp"
#include "OrbitingCamera.hpp"
#include "Config.hpp"
#include "TextureUtils.hpp"
#include "MillPathFormatReader.hpp"
#include "Shaders.hpp"
#include "CuttingToolGUI.hpp"
#include "CuttingToolModel.hpp"
#include "HeightmapGeometry.hpp"
#include "CuttingToolController.hpp"
#include "HeightmapVisualizationEffect.hpp"
#include "TexturedPhongEffect.hpp"
#include "HeightmapTextureConverter.hpp"
#include "MillingBlock.hpp"
#include "ProgramManagerGUI.hpp"
#include "MillingProgramExecutor.hpp"
#include "MillingProgramExecutorGUI.hpp"
#include "MillingBlockCreationWindow.hpp"
#include "IModeController.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

namespace ms
{

class SimulationModeController:
    public IModeController
{
public:
    SimulationModeController();
    virtual ~SimulationModeController();

    // todo: remove this
    void setWindow(GLFWwindow *window);

    virtual void onActivate();
    virtual void onDeactivate();
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onUpdate(float deltaTime);
    virtual void onRender(const OrbitingCamera &orbitingCamera);
    virtual void onKey(int key, int scancode, int action, int mods);
    virtual void updateMainMenuBar();

    std::shared_ptr<ms::MillingProgramExecutor> getProgramExecutor();

protected:
    GLFWwindow *_window;

    bool isGUIHidden();
    void updateMillingErrorPopup();

    void updateRenderables();
    void updateCuttingTool();

    bool isGeometryRenderingEnabled();
    void renderSceneGeometry(const OrbitingCamera &orbitingCamera);

private:
    TexturedPhongEffect _effect;
    fw::BasicEffect _basicEffect;
    std::shared_ptr<ms::MillingBlock> _block;
    GLuint _texture;
    bool _showProgramManager;
    bool _showMillingPath;
    unsigned int _heightmapResolutionX;
    unsigned int _heightmapResolutionY;
    bool _newBlockRequested;

    CuttingToolModel _cuttingTool;
    CuttingToolGUI _cuttingToolGUI;

    std::vector<ms::PathMovement> _movements;
    int _currentMoveIndex;
    ms::MillingError _lastErrorState;

    std::shared_ptr<ms::ProgramManagerGUI> _programManagerGUI;
    std::shared_ptr<ms::CuttingToolController> _toolController;
    std::shared_ptr<ms::MillingProgramExecutor> _programExecutor;
    std::shared_ptr<ms::MillingProgramExecutorGUI> _programExecutorGUI;
    std::shared_ptr<ms::MillingBlockCreationWindow> _createBlockGUI;
};

}
