#pragma once

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

#include "MillPathFormatReader.hpp"
#include "MillingProgramExecutor.hpp"
#include "PolygonalLine.hpp"

namespace ms
{

class ProgramEditor
{
public:
    ProgramEditor();

    bool *getVisibilityFlagPointer();
    void setVisibility(bool isVisible);
    void update(glm::mat4 view, glm::mat4 proj);
    void render();

protected:
    void discoverAvailableFiles();
    void loadSelectedFile();
    void recreatePreview();

private:
    boost::filesystem::path _observedDirectory;
    std::vector<boost::filesystem::path> _discoveredPaths;
    int _selectedIndex;
    bool _isOpened;
    bool _stepManipulationEnabled;

    std::vector<PathMovement> _loadedProgram;
    CuttingToolKind _toolKind;
    double _toolRadius;
    int _selectedProgramStep;

    std::shared_ptr<fw::PolygonalLine> _preview;

    std::string _targetFileName;
};

}
