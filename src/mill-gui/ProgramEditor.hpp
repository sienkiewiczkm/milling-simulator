#pragma once

#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include "OpenGLHeaders.hpp"

#include "MillPathFormatReader.hpp"
#include "MillingProgramExecutor.hpp"
#include "PolygonalLine.hpp"

namespace ms
{

class ProgramEditor
{
public:
    ProgramEditor(GLFWwindow *window);

    bool *getVisibilityFlagPointer();
    void setVisibility(bool isVisible);
    void update(glm::mat4 view, glm::mat4 proj);
    void render();

protected:
    void discoverAvailableFiles();
    void loadSelectedFile();
    void recreatePreview();

private:
    GLFWwindow *_window;
    boost::filesystem::path _observedDirectory;
    std::vector<boost::filesystem::path> _discoveredPaths;
    int _selectedIndex;
    bool _isOpened;
    bool _stepManipulationEnabled;
    bool _cloneKeyBlocked;

    std::vector<PathMovement> _loadedProgram;
    CuttingToolKind _toolKind;
    double _toolRadius;
    int _selectedProgramStep;

    float _workLevel;
    float _elevatedLevel;

    std::shared_ptr<fw::PolygonalLine> _preview;

    std::string _targetFileName;
};

}
