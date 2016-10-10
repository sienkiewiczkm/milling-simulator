#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "ImGuiBinding.h"

int main(void)
{
    if (!glfwInit())
    {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(
        640, 480, "Hello world", nullptr, nullptr
    );

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    ImGuiBinding::initialize(window, true);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGuiBinding::newFrame();

        static float f = 0.0f;
        ImGui::Text("Hello world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

		int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(1.0, 0.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    ImGuiBinding::shutdown();

    glfwTerminate();
    return 0;
}
