#include "ProgramManagerGUI.hpp"
#include "Config.hpp" 
#include <imgui.h>
#include <iostream>

#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace std;

namespace ms
{

ProgramManagerGUI::ProgramManagerGUI():
    _isOpened(false)
{
    _observedDirectory = RESOURCE("paths");
    //discoverAvailableFiles();
    directory_entry pathDirectory(_observedDirectory);
    _discoveredPaths.clear();
    for (auto &item : directory_iterator(pathDirectory))
    {
        if (item.path().filename().string()[0] == '.') { continue; }
        _discoveredPaths.push_back(item.path());
        cout << item.path() << endl;
    }
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
            ImGui::Button("Load");
            ImGui::EndChild();
        }

    }

    ImGui::End();
}

void ProgramManagerGUI::discoverAvailableFiles()
{
    directory_entry pathDirectory(_observedDirectory);
    _discoveredPaths.clear();
    for (auto &item : directory_iterator(pathDirectory))
    {
        if (item.path().filename().string()[0] == '.') { continue; }
        _discoveredPaths.push_back(item.path());
        cout << item.path() << endl;
    }
}

}
