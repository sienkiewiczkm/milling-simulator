#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class OpenGLApplication
{
public:
    OpenGLApplication();
    virtual ~OpenGLApplication();

    void create();
    void destroy();
    void run();

protected:
    virtual void onCreate();
    virtual void onDestroy();
    virtual void onUpdate();
    virtual void onRender();

    GLFWwindow *_window;
};
