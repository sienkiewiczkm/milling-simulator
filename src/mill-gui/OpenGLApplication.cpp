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

    _window = glfwCreateWindow(640, 480, "Hello world", nullptr, nullptr);

    if (!_window)
    {
        glfwTerminate();
        throw string("Cannot create GLFW3 window.");
    }
    
    glfwMakeContextCurrent(_window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSetWindowUserPointer(_window, static_cast<void*>(this));

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
