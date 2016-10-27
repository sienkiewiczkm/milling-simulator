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
    {
        ImGui::End();
        return;
    }

    if (ImGui::CollapsingHeader("Cutting tool parameters"))
    {
        CuttingToolParams params = _controller->getCuttingToolParams();

        int selectedItem = static_cast<int>(params.kind) - 1;
        const char *kinds[] = { "Ball", "Flat" };
        ImGui::Combo("Kind", &selectedItem, kinds, 2);
        params.kind = selectedItem == 0
            ? CuttingToolKind::Ball
            : CuttingToolKind::Flat;

        float radius = static_cast<float>(params.radius);
        ImGui::SliderFloat("Radius", &radius, 0.1f, 25.0f);

        params.radius = radius;
        _controller->setCuttingToolParams(params);
    }

    if (ImGui::CollapsingHeader("Additional information"))
    {
        vec3 startingPosition = _controller->getStartingPosition();
        vec3 currentPosition = _controller->getCurrentPosition();
        vec3 targetPosition = _controller->getTargetPosition();

        ImGui::InputFloat3(
            "Tip position", 
            glm::value_ptr(currentPosition), 
            -1, 
            ImGuiInputTextFlags_ReadOnly
        );

        ImGui::InputFloat3(
            "Starting position", 
            glm::value_ptr(startingPosition), 
            -1, 
            ImGuiInputTextFlags_ReadOnly
        );

        ImGui::InputFloat3(
            "Target position", 
            glm::value_ptr(targetPosition), 
            -1, 
            ImGuiInputTextFlags_ReadOnly
        );
        
        bool isMovementActive = _controller->isMovementActive();
        ImGui::Checkbox("Movement active", &isMovementActive);
    }

    ImGui::End();
}
