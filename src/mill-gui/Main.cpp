#include "ImGuiBinding.hpp"
#include "Shaders.hpp"
#include "Config.hpp"
#include "TextureUtils.hpp"
#include "DebugShapes.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <stb_image.h>

#include <iostream>
#include <memory>

using namespace std;

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

    shared_ptr<VertexShader> vs = make_shared<VertexShader>(
        RESOURCE("shaders/MVPTransformTexCoord.vert")
    );

    shared_ptr<FragmentShader> fs = make_shared<FragmentShader>(
        RESOURCE("shaders/SingleTexture.frag")
    );

    shared_ptr<ShaderProgram> program = make_shared<ShaderProgram>();
    program->attach(vs.get());
    program->attach(fs.get());
    program->link();

    GLuint texture = loadTextureFromFile(RESOURCE("textures/wefi_cat.jpg"));

    vector<GLfloat> vertices = createCube(1.0f, 1.0f, 1.0f);

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
        &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
            5 * sizeof(GLfloat),
            (GLvoid*)0
    );

    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
            5 * sizeof(GLfloat), 
            (GLvoid*)(3*sizeof(GLfloat))
    );

    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGuiBinding::newFrame();

        static bool booleanset;
        
        ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Simulation"))
            {
                ImGui::MenuItem("Clear simulation", NULL, &booleanset);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        if (ImGui::CollapsingHeader("Window options"))
        {
            static bool flag;
            ImGui::Checkbox("No titlebar", &flag);
        }

        ImGui::End();

        static float f = 0.0f;
        ImGui::Text("Hello world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

		int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(1.0, 0.0, 1.0, 1.0);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program->getId(), "TextureSlot1"), 0);

		glUseProgram(program->getId());

        glm::mat4 model, view, projection;

        float aspectRatio = (float)display_w/display_h;
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 1.0f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(45.0f, aspectRatio, 0.1f, 100.0f);

        GLint modelLoc = glGetUniformLocation(program->getId(), "model");
        GLint viewLoc = glGetUniformLocation(program->getId(), "view");
        GLint projLoc = glGetUniformLocation(program->getId(), "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
        glBindVertexArray(0);

        ImGui::Render();
        glfwSwapBuffers(window);
    }

    ImGuiBinding::shutdown();

    glfwTerminate();
    return 0;
}
