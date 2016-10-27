#include "OpenGLApplication.hpp"
#include <string>

using namespace std;

OpenGLApplication::OpenGLApplication()
{
}

OpenGLApplication::~OpenGLApplication()
{
    destroy();
}

void OpenGLApplication::create()
{
    if (!glfwInit())
    {
        throw string("Cannot initialize GLFW3.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    _window = glfwCreateWindow(640, 480, "Milling Simulator", nullptr, 
        nullptr);

    if (!_window)
    {
        glfwTerminate();
        throw string("Cannot create GLFW3 window.");
    }
    
    glfwMakeContextCurrent(_window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSetWindowUserPointer(_window, static_cast<void*>(this));

    glfwSetMouseButtonCallback(_window, OpenGLApplication::mouseButtonCallback);
    glfwSetScrollCallback(_window, OpenGLApplication::scrollCallback);
    glfwSetKeyCallback(_window, OpenGLApplication::keyCallback);
    glfwSetCharCallback(_window, OpenGLApplication::charCallback);

    onCreate();
}

void OpenGLApplication::destroy()
{
    onDestroy();
    glfwTerminate();
}

void OpenGLApplication::run()
{
    while (!glfwWindowShouldClose(_window))
    {
        glfwPollEvents();
        onUpdate();

        onRender();
        glfwSwapBuffers(_window);
    }

    destroy();
}

void OpenGLApplication::onCreate()
{
}

void OpenGLApplication::onDestroy()
{
}

void OpenGLApplication::onUpdate()
{
}

void OpenGLApplication::onRender()
{
}

void OpenGLApplication::onMouseButton(int button, int action, int mods)
{
}

void OpenGLApplication::onScroll(double xoffset, double yoffset)
{
}

void OpenGLApplication::onKey(int key, int scancode, int action, int mods)
{
}

void OpenGLApplication::onChar(unsigned int c)
{
}

void OpenGLApplication::mouseButtonCallback(
    GLFWwindow *window, int button, int action, int mods
)
{
    auto app = static_cast<OpenGLApplication*>(
        glfwGetWindowUserPointer(window)
    );

    app->onMouseButton(button, action, mods);
}

void OpenGLApplication::scrollCallback(
    GLFWwindow *window, double xoffset, double yoffset
)
{
    auto app = static_cast<OpenGLApplication*>(
        glfwGetWindowUserPointer(window)
    );

    app->onScroll(xoffset, yoffset);
}

void OpenGLApplication::keyCallback(
    GLFWwindow *window, int key, int scancode, int action, int mods
)
{
    auto app = static_cast<OpenGLApplication*>(
        glfwGetWindowUserPointer(window)
    );

    app->onKey(key, scancode, action, mods);
}

void OpenGLApplication::charCallback(GLFWwindow *window, unsigned int c)
{
    auto app = static_cast<OpenGLApplication*>(
        glfwGetWindowUserPointer(window)
    );

    app->onChar(c);
}

glm::vec2 OpenGLApplication::getMousePosition()
{
    double xPos, yPos;
    glfwGetCursorPos(_window, &xPos, &yPos);
    return glm::vec2((float)xPos, (float)yPos);
}

