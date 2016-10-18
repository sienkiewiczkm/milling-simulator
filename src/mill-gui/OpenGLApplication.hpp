#pragma once

#include "OpenGLHeaders.hpp"
#include <glm/glm.hpp>

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

    virtual void onMouseButton(int button, int action, int mods);
    virtual void onScroll(double xoffset, double yoffset);
    virtual void onKey(int key, int scancode, int action, int mods);
    virtual void onChar(unsigned int c);

    static void mouseButtonCallback(
        GLFWwindow *window, int button, int action, int mods
    );

    static void scrollCallback(
        GLFWwindow *window, double xoffset, double yoffset
    );

    static void keyCallback(
        GLFWwindow *window, int key, int scancode, int action, int mods
    );

    static void charCallback(GLFWwindow *window, unsigned int c);

    glm::vec2 getMousePosition();

    GLFWwindow *_window;
};
