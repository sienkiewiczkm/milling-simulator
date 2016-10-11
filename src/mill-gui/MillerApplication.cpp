#include "MillerApplication.hpp"
#include "Shaders.hpp"
#include "ImGuiBinding.hpp"
#include "Config.hpp"
#include "TextureUtils.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <cmath>

using namespace std;

MillerApplication::~MillerApplication()
{
}

void MillerApplication::onCreate()
{
    ImGuiBinding::initialize(_window, true);

    shared_ptr<VertexShader> vs = make_shared<VertexShader>(
        RESOURCE("shaders/MVPTransformTexCoord.vert")
    );

    shared_ptr<FragmentShader> fs = make_shared<FragmentShader>(
        RESOURCE("shaders/SingleTexture.frag")
    );

    _program = make_shared<ShaderProgram>();
    _program->attach(vs.get());
    _program->attach(fs.get());
    _program->link();

    _texture = loadTextureFromFile(RESOURCE("textures/wefi_cat.jpg"));

    const int heightmapWidth = 64;
    const int heightmapHeight = 64;
    vector<float> heightmap;

    for (int y = 0; y < heightmapHeight; ++y)
    {
        float yfactor = sinf(y*0.1f);
        for (int x = 0; x < heightmapWidth; ++x)
        {
            float xfactor = sinf(x*0.1f);
            float factor = 0.1f *((xfactor+yfactor) / 2.0f + 1.0f);
            heightmap.push_back(factor);
        }
    }

    _heightmapGeo.create(heightmapWidth, heightmapHeight, heightmap);

}

void MillerApplication::onDestroy()
{
    ImGuiBinding::shutdown();
}

void MillerApplication::onUpdate()
{
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
}

void MillerApplication::onRender()
{
    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(1.0, 0.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glUniform1i(glGetUniformLocation(_program->getId(), "TextureSlot1"), 0);

    glUseProgram(_program->getId());

    glm::mat4 model, view, projection;

    float aspectRatio = (float)display_w/display_h;
    model = glm::rotate(
        model,
        (float)glfwGetTime(), 
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(45.0f, aspectRatio, 0.1f, 100.0f);

    GLint modelLoc = glGetUniformLocation(_program->getId(), "model");
    GLint viewLoc = glGetUniformLocation(_program->getId(), "view");
    GLint projLoc = glGetUniformLocation(_program->getId(), "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    _heightmapGeo.render();

    ImGui::Render();
}
