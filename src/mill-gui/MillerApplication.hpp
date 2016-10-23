#pragma once

#include "CuttingToolGUI.hpp"
#include "CuttingToolModel.hpp"
#include "HeightmapGeometry.hpp"
#include "OpenGLApplication.hpp"
#include "OrbitingCamera.hpp"
#include "TexturedPhongEffect.hpp"
#include "CuttingToolController.hpp"
#include "MillPathFormatReader.hpp"
#include "HeightmapVisualizationEffect.hpp"
#include "HeightmapTextureConverter.hpp"
#include "MillingBlock.hpp"
#include "ProgramManagerGUI.hpp"
#include "MillingProgramExecutor.hpp"
#include "MillingProgramExecutorGUI.hpp"
#include "MillingBlockCreationWindow.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class ShaderProgram;

class MillerApplication :
    public OpenGLApplication
{
public:
    MillerApplication();
    virtual ~MillerApplication();

protected:
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onUpdate();
    virtual void onRender();

    virtual void onMouseButton(int button, int action, int mods);
    virtual void onScroll(double xoffset, double yoffset);
    virtual void onKey(int key, int scancode, int action, int mods);
    virtual void onChar(unsigned int c);

    void handleInput();

protected:
    void updateMainMenuBar();

private:
    unsigned int _heightmapResolutionX;
    unsigned int _heightmapResolutionY;

    unsigned int _frame;

    bool _newBlockRequested;

    float _mouseSensitivity;
    glm::vec2 _lastMousePosition;

    TexturedPhongEffect _effect;

    std::shared_ptr<ms::MillingBlock> _block;

    GLuint _texture;

    bool _showImguiDemo;
    bool _showProgramManager;

    OrbitingCamera _camera;
    std::shared_ptr<ms::ProgramManagerGUI> _programManagerGUI;
    std::shared_ptr<ms::CuttingToolController> _toolController;
    std::shared_ptr<ms::MillingProgramExecutor> _programExecutor;
    std::shared_ptr<ms::MillingProgramExecutorGUI> _programExecutorGUI;
    std::shared_ptr<ms::MillingBlockCreationWindow> _createBlockGUI;

    CuttingToolModel _cuttingTool;
    CuttingToolGUI _cuttingToolGUI;

    std::vector<ms::PathMovement> _movements;
    int _currentMoveIndex;

    double _lastTime;
    double _currentTime;
    double _deltaTime;
    ms::MillingError _lastErrorState;
};
