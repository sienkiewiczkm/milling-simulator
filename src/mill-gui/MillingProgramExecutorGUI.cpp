#include "MillingProgramExecutorGUI.hpp"
#include "MillingProgramExecutor.hpp"

#include <imgui.h>

#include <sstream>

using namespace std;

namespace ms
{

MillingProgramExecutorGUI::MillingProgramExecutorGUI(
    std::shared_ptr<MillingProgramExecutor> programExecutor
):
    _startFastForward(false)
{
    _programExecutor = programExecutor;
}

MillingProgramExecutorGUI::~MillingProgramExecutorGUI()
{
}

void MillingProgramExecutorGUI::update()
{
    if (ImGui::Begin("Milling program executor"))
    {
        showExecutionControls();
        showProgressBars();
        showFastForwardPopup();
    }

    ImGui::End();
}

void MillingProgramExecutorGUI::showExecutionControls()
{
    ImGui::Text("Loaded program: ");
    ImGui::SameLine();
    auto programName = _programExecutor->isProgramLoaded()
        ? _programExecutor->getProgramName()
        : "None";
    ImGui::Text(programName.c_str());

    if (ImGui::Button("Start")) { _programExecutor->start(); }
    ImGui::SameLine();
    if (ImGui::Button("Pause")) { _programExecutor->stop(); }
    ImGui::SameLine();
    if (ImGui::Button("Fast-forward")) { _startFastForward = true; }

    auto controller = _programExecutor->getController();
    float speed = static_cast<float>(controller->getMovementSpeed());
    ImGui::SliderFloat("Speed", &speed, 0.0f, 500.0f);
    controller->setMovementSpeed(speed);
}

void MillingProgramExecutorGUI::showFastForwardPopup()
{
    if (_startFastForward)
    {
        _startFastForward = false;
        ImGui::OpenPopup("Fast-forwarding");
    }

    if (ImGui::BeginPopupModal(
            "Fast-forwarding",
            nullptr,
            ImGuiWindowFlags_AlwaysAutoResize
        ))
    {
        ImGui::Text("Fast-forwarding the milling process. Please, be patient.");
        showCommandsProgress();
        if (_programExecutor->isInFastForwardMode())
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void MillingProgramExecutorGUI::showProgressBars()
{
    showCommandsProgress();
    showMoveProgress();
}

void MillingProgramExecutorGUI::showCommandsProgress()
{
    auto finishedSteps = _programExecutor->getExecutedStepsNum();
    auto totalSteps = _programExecutor->getTotalStepsNum();
    auto stepsProgress = totalSteps > 0
        ? static_cast<float>(finishedSteps) / totalSteps
        : 1.0f;

    stringstream ss;
    ss << finishedSteps << "/" << totalSteps;
    ImGui::ProgressBar(stepsProgress, ImVec2(0.0f, 0.0f), ss.str().c_str());
    ImGui::SameLine();
    ImGui::Text("Commands done");
}

void MillingProgramExecutorGUI::showMoveProgress()
{
    auto movePercent = _programExecutor->getMoveProgress();
    ImGui::ProgressBar(movePercent, ImVec2(0.0f, 0.0f));
    ImGui::SameLine();
    ImGui::Text("Movement completion");
}

}
