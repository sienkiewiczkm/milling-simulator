#include "ProgramEditor.hpp"
#include <array>
#include <iostream>
#include <sstream>

#include "Config.hpp"
#include "MillPathFormatReader.hpp"
#include "MillingProgramExecutor.hpp"
#include "ImGuiExtensions.hpp"

#include "imgui.h"
#include "ImGuizmo.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace std;

namespace ms
{

ProgramEditor::ProgramEditor(GLFWwindow *window):
    _isOpened(true),
    _selectedIndex(-1),
    _selectedProgramStep(-1),
    _workLevel{21.5f},
    _elevatedLevel{25.5f},
    _cloneKeyBlocked{false}
{
    _observedDirectory = RESOURCE("paths");
    discoverAvailableFiles();
    _window = window;
}

bool *ProgramEditor::getVisibilityFlagPointer()
{
    return &_isOpened;
}

void ProgramEditor::setVisibility(bool isVisible)
{
    _isOpened = isVisible;
}

void ProgramEditor::update(glm::mat4 view, glm::mat4 proj)
{
    if (!_isOpened) { return; }

    if (!ImGui::Begin("Program Editor", &_isOpened))
    {
        ImGui::End();
        return;
    }

    ImGui::InputFloat("Work level", &_workLevel);
    ImGui::InputFloat("Elevated level", &_elevatedLevel);

    ImGui::BeginChild(
        "Programs",
        ImVec2(ImGui::GetWindowWidth(), 100.0f),
        true
    );

    for (auto i = 0; i < _discoveredPaths.size(); ++i)
    {
        auto filename = _discoveredPaths[i].filename().c_str();
        if (ImGui::Selectable(filename, _selectedIndex == i))
            _selectedIndex = i;
    }

    ImGui::EndChild();

    if (ImGui::Button("Refresh")) { discoverAvailableFiles(); }
    ImGui::SameLine();
    if (ImGui::Button("Load")) { loadSelectedFile(); }

    std::array<char, 256> arrTargetFileName{};
    std::copy(
        std::begin(_targetFileName),
        std::end(_targetFileName),
        std::begin(arrTargetFileName)
    );

    ImGui::InputText("Output filename", arrTargetFileName.data(), 256);
    _targetFileName = arrTargetFileName.data();

    if (ImGui::Button("Save to file"))
    {
        auto saveFilePath =
            std::string(RESOURCE("paths/")) +  _targetFileName;
        MillPathFormatWriter writer;
        writer.writeToFile(saveFilePath, _loadedProgram);
    }

    std::vector<std::string> programStepsList{};
    for (auto i = 0; i < _loadedProgram.size(); ++i)
    {
        std::stringstream ss;
        ss << i + 1 << ": " << glm::to_string(_loadedProgram[i].position);
        programStepsList.push_back(ss.str());
    }

    ImGui::ListBox("Program steps", &_selectedProgramStep, programStepsList);

    if (ImGui::Button("Append new"))
    {
        _loadedProgram.push_back({});
        recreatePreview();
    }

    ImGui::SameLine();

    auto cloneButton = glfwGetKey(_window, GLFW_KEY_C);
    bool pleaseClone = cloneButton == GLFW_PRESS && !_cloneKeyBlocked;
    if (cloneButton != GLFW_PRESS) { _cloneKeyBlocked = false; }
    else { _cloneKeyBlocked = true; }

    if (_selectedIndex >= 0
        && (pleaseClone || ImGui::Button("Clone")))
    {
        auto selectedIterator =
            std::begin(_loadedProgram) + _selectedProgramStep;
        auto selectedCopy = *selectedIterator;
        _loadedProgram.insert(selectedIterator, selectedCopy);
        ++_selectedProgramStep;
        recreatePreview();
    }

    ImGui::SameLine();

    if (_selectedProgramStep > 0 && ImGui::Button("Move up"))
    {
        auto selectedIterator =
            std::begin(_loadedProgram) + _selectedProgramStep;
        auto prevIterator = std::prev(selectedIterator);
        std::swap(*selectedIterator, *prevIterator);
        --_selectedProgramStep;
        recreatePreview();
    }

    ImGui::SameLine();

    if (_selectedProgramStep >= 0
        && _selectedProgramStep + 1 < _loadedProgram.size()
        && ImGui::Button("Move down"))
    {
        auto selectedIterator =
            std::begin(_loadedProgram) + _selectedProgramStep;
        auto nextIterator = std::next(selectedIterator);
        std::swap(*selectedIterator, *nextIterator);
        ++_selectedProgramStep;
        recreatePreview();
    }

    ImGui::SameLine();

    if (_selectedProgramStep >= 0 && ImGui::Button("Delete"))
    {
        auto selectedIterator =
            std::begin(_loadedProgram) + _selectedProgramStep;
        _loadedProgram.erase(selectedIterator);
        _selectedProgramStep = std::min(
            _selectedProgramStep,
            static_cast<int>(_loadedProgram.size()) - 1
        );
        recreatePreview();
    }

    ImGui::Separator();

    if (_selectedIndex >= 0 && ImGui::Button("Clone & pre-elevate"))
    {
        auto selectedIterator =
            std::begin(_loadedProgram) + _selectedProgramStep;
        auto selectedCopy = *selectedIterator;
        _loadedProgram.insert(selectedIterator, selectedCopy);
        selectedIterator->position.y = _elevatedLevel;
        ++_selectedProgramStep;
        recreatePreview();
    }

    if (_selectedIndex >= 0 && ImGui::Button("Clone & post-elevate"))
    {
        auto selectedIterator =
            std::begin(_loadedProgram) + _selectedProgramStep;
        auto selectedCopy = *selectedIterator;
        selectedCopy.position.y = _elevatedLevel;
        _loadedProgram.insert(selectedIterator, selectedCopy);
        ++_selectedProgramStep;
        recreatePreview();
    }

    ImGui::Checkbox("Enable manipulation", &_stepManipulationEnabled);

    if (_selectedProgramStep >= 0
        && _selectedProgramStep < _loadedProgram.size())
    {
        if (!_stepManipulationEnabled)
        {
            if (ImGui::DragFloat3(
                "Position",
                glm::value_ptr(_loadedProgram[_selectedProgramStep].position),
                0.1f
            ))
            {
                recreatePreview();
            }
        }
        else
        {
            ImGui::Text("Step manipulation enabled - direct access disabled");

            auto selectedIterator =
                std::begin(_loadedProgram) + _selectedProgramStep;
            glm::mat4 translation = glm::translate(
                {},
                selectedIterator->position
            );

            ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(proj),
                ImGuizmo::TRANSLATE,
                ImGuizmo::LOCAL,
                glm::value_ptr(translation),
                nullptr,
                nullptr
            );

            glm::vec3 matrixTranslation, matrixRotation, matrixScale;
            ImGuizmo::DecomposeMatrixToComponents(
                glm::value_ptr(translation),
                glm::value_ptr(matrixTranslation),
                glm::value_ptr(matrixRotation),
                glm::value_ptr(matrixScale)
            );

            selectedIterator->position = matrixTranslation;
            recreatePreview();
        }
    }

    if (_stepManipulationEnabled)
    {
    }

    ImGui::End();

    ImGui::ShowTestWindow();
}

void ProgramEditor::render()
{
    if (_preview != nullptr) { _preview->render(); }
}

void ProgramEditor::discoverAvailableFiles()
{
    _selectedIndex = -1;
    _selectedProgramStep = -1;
    _discoveredPaths.clear();

    directory_entry pathDirectory(_observedDirectory);
    for (auto &item : directory_iterator(pathDirectory))
    {
        if (item.path().filename().string()[0] == '.') { continue; }
        _discoveredPaths.push_back(item.path());
    }
}

void ProgramEditor::loadSelectedFile()
{
    if (_selectedIndex == -1) { return; }

    auto &selected = _discoveredPaths[_selectedIndex];

    MillPathFormatReader reader;
    reader.readFromFile(selected.string());
    _loadedProgram = reader.getMovements();

    auto ext = selected.extension().string();
    if (ext[1] == 'k') { _toolKind = CuttingToolKind::Ball; }
    if (ext[1] == 'f') { _toolKind = CuttingToolKind::Flat; }

    auto diameter = stoi(ext.substr(2));
    _toolRadius = 0.5f * diameter;

    recreatePreview();
}

void ProgramEditor::recreatePreview()
{
    _preview = nullptr;
    if (_loadedProgram.size() == 0)
    {
        return;
    }

    std::vector<fw::VertexColor> points{};
    for (const auto &pos: _loadedProgram)
    {
        points.push_back({pos.position, {1.0f, 0.0f, 0.0f}});
    }

    _preview = std::make_shared<fw::PolygonalLine>(points);
}

}
