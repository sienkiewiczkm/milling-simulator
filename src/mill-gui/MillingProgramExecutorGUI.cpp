#include "MillingProgramExecutorGUI.hpp"
#include "MillingProgramExecutor.hpp"

#include <imgui.h>

#include <sstream>

using namespace std;

namespace ms
{

MillingProgramExecutorGUI::MillingProgramExecutorGUI(
    std::shared_ptr<MillingProgramExecutor> programExecutor
)
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
    }

    ImGui::End();
}

void MillingProgramExecutorGUI::showExecutionControls()
{
    if (ImGui::Button("Start")) { _programExecutor->start(); }
    ImGui::SameLine();
    if (ImGui::Button("Pause")) { _programExecutor->stop(); }

    auto controller = _programExecutor->getController();
    float speed = static_cast<float>(controller->getMovementSpeed());
    ImGui::SliderFloat("Speed", &speed, 0.0f, 500.0f);
    controller->setMovementSpeed(speed);
}

void MillingProgramExecutorGUI::showProgressBars()
{
    auto finishedSteps = _programExecutor->getExecutedStepsNum();
    auto totalSteps = _programExecutor->getTotalStepsNum();
    auto movePercent = _programExecutor->getMoveProgress();
    auto stepsProgress = static_cast<float>(finishedSteps) / totalSteps;

    stringstream ss;
    ss << finishedSteps << "/" << totalSteps;
    ImGui::ProgressBar(stepsProgress, ImVec2(0.0f, 0.0f), ss.str().c_str());
    ImGui::SameLine();
    ImGui::Text("Commands done");

    ImGui::ProgressBar(movePercent, ImVec2(0.0f, 0.0f));
    ImGui::SameLine();
    ImGui::Text("Movement completion");
}

}
