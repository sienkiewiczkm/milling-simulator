#include "MillingBlockCreationWindow.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

using namespace fw;

namespace ms
{

MillingBlockCreationWindow::MillingBlockCreationWindow():
    _size(150.0f, 50.0f, 150.0f),
    _resolution(1024, 1024),
    _safeHeight(20.0f)
{
}


MillingBlockCreationWindow::~MillingBlockCreationWindow()
{
}

void MillingBlockCreationWindow::open()
{
    ImGui::OpenPopup("New block");
}

void MillingBlockCreationWindow::update()
{
    if (ImGui::BeginPopupModal(
            "New block",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize
        ))
    {

        ImGui::SliderFloat3("Size", value_ptr(_size), 0.0f, 1000.0f);
        ImGui::SliderInt2("Resolution", value_ptr(_resolution), 0, 2500);
        ImGui::SliderFloat("Safe height", &_safeHeight, 0.0f, _size.y);

        ImGui::Separator();

        ImGui::TextColored(
            ImVec4(1.0f, 0.0f, 0.0f, 1.0f),
            "Current block will be lost."
        );

        if (ImGui::Button("OK"))
        { 
            createMillingBlock();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }

        ImGui::EndPopup();
    }
}

void MillingBlockCreationWindow::createMillingBlock()
{
    /* todo */
}

}
