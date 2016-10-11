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

private:
    std::shared_ptr<ShaderProgram> _program;
    GLuint _texture;
    HeightmapGeometry _heightmapGeo;
};
