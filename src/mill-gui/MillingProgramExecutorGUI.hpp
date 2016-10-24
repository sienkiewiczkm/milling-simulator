#pragma once

#include <memory>

namespace ms
{
class MillingProgramExecutor;

class MillingProgramExecutorGUI
{
public:
    MillingProgramExecutorGUI(
        std::shared_ptr<MillingProgramExecutor> programExecutor
    );

    ~MillingProgramExecutorGUI();

    void update();

protected:
    void showExecutionControls();
    void showFastForwardPopup();
    void showProgressBars();
    void showCommandsProgress();
    void showMoveProgress();

private:
    bool _startFastForward;
    std::shared_ptr<MillingProgramExecutor> _programExecutor;
};

}
