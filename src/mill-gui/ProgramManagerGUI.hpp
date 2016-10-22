#pragma once

#include <vector>
#include <boost/filesystem.hpp>

namespace ms
{

class ProgramManagerGUI
{
public:
    ProgramManagerGUI();

    bool *getVisibilityFlagPointer();
    void setVisibility(bool isVisible);
    void update();

protected:
    void discoverAvailableFiles();

private:
    boost::filesystem::path _observedDirectory;
    std::vector<boost::filesystem::path> _discoveredPaths;
    int _selectedIndex;
    bool _isOpened;
};

}
