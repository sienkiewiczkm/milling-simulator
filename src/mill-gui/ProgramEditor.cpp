#include "ProgramEditor.hpp"
#include <sstream>

#include "Config.hpp"
#include "MillPathFormatReader.hpp"
#include "MillingProgramExecutor.hpp"
#include "ImGuiExtensions.hpp"

#include <imgui.h>
#include <array>
#include <iostream>

#define GLM_USE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace std;

namespace ms
{

ProgramEditor::ProgramEditor():
    _isOpened(true),
    _selectedIndex(-1),
    _selectedProgramStep(-1)
{
    _observedDirectory = RESOURCE("paths");
    discoverAvailableFiles();
}

bool *ProgramEditor::getVisibilityFlagPointer()
{
    return &_isOpened;
}

void ProgramEditor::setVisibility(bool isVisible)
{
    _isOpened = isVisible;
}

void ProgramEditor::update()
{
    if (!_isOpened) { return; }

    if (!ImGui::Begin("Program Editor", &_isOpened))
    {
        ImGui::End();
        return;
    }

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
    }

    ImGui::SameLine();

    if (_selectedIndex >= 0 && ImGui::Button("Clone"))
    {
        auto selectedIterator =
            std::begin(_loadedProgram) + _selectedProgramStep;
        auto selectedCopy = *selectedIterator;
        _loadedProgram.insert(selectedIterator, selectedCopy);
        ++_selectedProgramStep;
    }

    ImGui::SameLine();

    if (_selectedProgramStep > 0 && ImGui::Button("Move up"))
    {
        auto selectedIterator =
            std::begin(_loadedProgram) + _selectedProgramStep;
        auto prevIterator = std::prev(selectedIterator);
        std::swap(*selectedIterator, *prevIterator);
        --_selectedProgramStep;
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
    }

    ImGui::End();

    ImGui::ShowTestWindow();
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
}

}
