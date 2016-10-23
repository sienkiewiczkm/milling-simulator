#pragma once

#include <vector>
#include <boost/filesystem.hpp>

namespace ms
{

class MillingProgramExecutor;

class ProgramManagerGUI
{
public:
    ProgramManagerGUI(
        std::shared_ptr<MillingProgramExecutor> programExecutor
    );

    bool *getVisibilityFlagPointer();
    void setVisibility(bool isVisible);
    void update();

protected:
    void discoverAvailableFiles();
    void loadSelectedFile();

private:
    boost::filesystem::path _observedDirectory;
    std::vector<boost::filesystem::path> _discoveredPaths;
    int _selectedIndex;
    bool _isOpened;

    std::shared_ptr<MillingProgramExecutor> _programExecutor;
};

}
