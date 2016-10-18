#pragma once

#include "CuttingToolGUI.hpp"
#include "CuttingToolModel.hpp"
#include "HeightmapGeometry.hpp"
#include "OpenGLApplication.hpp"
#include "OrbitingCamera.hpp"
#include "TexturedPhongEffect.hpp"
#include "CuttingToolController.hpp"

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

    std::vector<float> generateHeightmap(int width, int height);
    void handleInput();

private:
    float _mouseSensitivity;
    glm::vec2 _lastMousePosition;
    TexturedPhongEffect _effect;
    GLuint _texture;
    HeightmapGeometry _heightmapGeo;
    OrbitingCamera _camera;
    std::shared_ptr<ms::CuttingToolController> _toolController;

    CuttingToolModel _cuttingTool;
    CuttingToolGUI _cuttingToolGUI;

    double _lastTime;
    double _currentTime;
    double _deltaTime;
};
