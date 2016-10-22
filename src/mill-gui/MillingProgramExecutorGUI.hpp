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
    void showProgressBars();

private:
    std::shared_ptr<MillingProgramExecutor> _programExecutor;
};

}
