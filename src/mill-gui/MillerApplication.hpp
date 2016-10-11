#pragma once
#include "OpenGLApplication.hpp"
#include "HeightmapGeometry.hpp"

#include <memory>

class ShaderProgram;

class MillerApplication :
    public OpenGLApplication
{
public:
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

private:
    std::shared_ptr<ShaderProgram> _program;
    GLuint _texture;
    HeightmapGeometry _heightmapGeo;
};
