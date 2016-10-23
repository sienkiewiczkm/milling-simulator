#pragma once

#include "MillPathFormatReader.hpp"
#include "CuttingToolController.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ms
{

class MillingProgramExecutor
{
public:
    MillingProgramExecutor(
        std::shared_ptr<CuttingToolController> cuttingToolController
    );
    ~MillingProgramExecutor();

    const std::string &getProgramName() const;
    void setProgram(
        const std::string &programName,
        const std::vector<ms::PathMovement> &millingProgram
    );

    bool isProgramLoaded();

    std::shared_ptr<CuttingToolController> getController();

    void start();
    void stop();

    double getMoveProgress();
    int getExecutedStepsNum();
    int getTotalStepsNum();

    void update(double dt);

private:
    std::string _programName;
    bool _isRunning;
    int _currentProgramStep;
    std::vector<ms::PathMovement> _millingProgram;
    std::shared_ptr<CuttingToolController> _toolController;
};

}
