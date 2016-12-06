#pragma once

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#include "MillPathFormatReader.hpp"
#include "MillingProgramExecutor.hpp"

namespace ms
{

class ProgramEditor
{
public:
    ProgramEditor();

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

    std::vector<PathMovement> _loadedProgram;
    CuttingToolKind _toolKind;
    double _toolRadius;
    int _selectedProgramStep;

    std::string _targetFileName;
};

}
