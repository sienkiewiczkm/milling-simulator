#include "CuttingToolGUI.hpp"
#include "CuttingToolController.hpp"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <sstream>

using namespace std;
using namespace glm;
using namespace ms;

CuttingToolGUI::CuttingToolGUI()
{
}

void CuttingToolGUI::setVisibility(bool isVisible)
{
    _isVisible = isVisible;
}

void CuttingToolGUI::setWindowName(const std::string &windowName)
{
    _windowName = windowName;
}

void CuttingToolGUI::setController(
    std::shared_ptr<CuttingToolController> controller
)
{
    _controller = controller;
}

void CuttingToolGUI::update()
{
    if (!ImGui::Begin(_windowName.c_str(), &_isVisible))
        return;

    vec3 currentPosition = _controller->getCurrentPosition();

    ImGui::InputFloat3(
        "Tip position", 
        glm::value_ptr(currentPosition), 
        -1, 
        ImGuiInputTextFlags_ReadOnly
    );

    ImGui::InputFloat3("Desired position", glm::value_ptr(_desiredPosition));
    ImGui::Button("Goto");

    struct ToolAxis
    {
        std::string axisName;
        glm::vec3 axisPositive;
    };

    std::vector<ToolAxis> axes = {
        { "X", vec3(1.0f, 0.0f, 0.0f) },
        { "Y", vec3(0.0f, 1.0f, 0.0f) },
        { "Z", vec3(0.0f, 0.0f, 1.0f) }
    };

    std::vector<float> multiplers = { -1.0f, +1.0f };

    for (auto &axis : axes)
    {
        for (auto multipler : multiplers)
        {
            stringstream ss;
            ss << (multipler > 0 ? '+' : '-');
            ss << axis.axisName;
            ImGui::Button(ss.str().c_str());
            ImGui::SameLine();
        }
    }

    ImGui::End();
}
