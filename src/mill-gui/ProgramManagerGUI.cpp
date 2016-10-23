#include "ProgramManagerGUI.hpp"
#include "Config.hpp" 
#include "MillPathFormatReader.hpp"
#include "MillingProgramExecutor.hpp"

#include <imgui.h>
#include <iostream>

#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace std;

namespace ms
{

ProgramManagerGUI::ProgramManagerGUI(
    std::shared_ptr<MillingProgramExecutor> programExecutor
):
    _programExecutor(programExecutor),
    _isOpened(false),
    _selectedIndex(-1)
{
    _observedDirectory = RESOURCE("paths");
    discoverAvailableFiles();
}

bool *ProgramManagerGUI::getVisibilityFlagPointer()
{
    return &_isOpened;
}

void ProgramManagerGUI::setVisibility(bool isVisible)
{
    _isOpened = isVisible;
}

void ProgramManagerGUI::update()
{
    if (!_isOpened) { return; }

    if (ImGui::Begin("Milling Program Manager", &_isOpened))
    {
        if (ImGui::BeginChild("Left Pane", ImVec2(150, 0), true))
        {
            for (auto i = 0; i < _discoveredPaths.size(); ++i)
            {
                auto filename = _discoveredPaths[i].filename().c_str();
                if (ImGui::Selectable(filename, _selectedIndex == i))
                    _selectedIndex = i;
            }

            ImGui::EndChild();
        }
        ImGui::SameLine();

        if (ImGui::BeginChild("Right Pane"))
        {
            if (ImGui::Button("Refresh")) { discoverAvailableFiles(); }
            if (ImGui::Button("Load")) { loadSelectedFile(); }
            ImGui::EndChild();
        }

    }

    ImGui::End();
}

void ProgramManagerGUI::discoverAvailableFiles()
{
    _selectedIndex = -1;
    _discoveredPaths.clear();

    directory_entry pathDirectory(_observedDirectory);
    for (auto &item : directory_iterator(pathDirectory))
    {
        if (item.path().filename().string()[0] == '.') { continue; }
        _discoveredPaths.push_back(item.path());
        cout << item.path() << endl;
    }
}

void ProgramManagerGUI::loadSelectedFile()
{
    if (_selectedIndex == -1) { return; }

    auto &selected = _discoveredPaths[_selectedIndex];

    MillPathFormatReader reader;
    reader.readFromFile(selected.string());
    auto loadedProgram = reader.getMovements();
    _programExecutor->setProgram(selected.filename().string(), loadedProgram);

    CuttingToolParams defaultParameters;
    auto ext = selected.extension().string();

    if (ext[1] == 'k') { defaultParameters.kind = CuttingToolKind::Ball; }
    if (ext[1] == 'f') { defaultParameters.kind = CuttingToolKind::Flat; }
    auto diameter = stoi(ext.substr(2));
    defaultParameters.radius = 0.5f * diameter;
    _programExecutor->getController()->setCuttingToolParams(defaultParameters);
}

}
