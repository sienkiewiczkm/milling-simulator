#pragma once

#include "MillPathFormatReader.hpp"
#include "CuttingToolController.hpp"

#include <memory>
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

    void setProgram(const std::vector<ms::PathMovement> millingProgram);
    std::shared_ptr<CuttingToolController> getController();

    void start();
    void stop();

    double getMoveProgress();
    int getExecutedStepsNum();
    int getTotalStepsNum();

    void update(double dt);

private:
    bool _isRunning;
    int _currentProgramStep;
    std::vector<ms::PathMovement> _millingProgram;
    std::shared_ptr<CuttingToolController> _toolController;
};

}
