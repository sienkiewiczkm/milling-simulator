#pragma once

#include "MillPathFormatReader.hpp"
#include "CuttingToolController.hpp"
#include "MillingBlock.hpp"
#include "MillingErrors.hpp"
#include "PolygonalLine.hpp"

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

    std::shared_ptr<MillingBlock> getMillingBlock() const;
    void setMillingBlock(std::shared_ptr<MillingBlock> block);

    void start();
    void stop();

    double getMoveProgress();
    int getExecutedStepsNum();
    int getTotalStepsNum();

    void enableFastForward();
    bool isInFastForwardMode();

    MillingError update(double dt);
    std::shared_ptr<fw::PolygonalLine> getProgramPolygonalLine();

private:
    bool _fastForwardMode;
    std::string _programName;
    bool _isRunning;
    int _currentProgramStep;
    std::vector<ms::PathMovement> _millingProgram;
    std::shared_ptr<CuttingToolController> _toolController;
    std::shared_ptr<MillingBlock> _block;
    std::shared_ptr<fw::PolygonalLine> _polygonalLine;
};

}
