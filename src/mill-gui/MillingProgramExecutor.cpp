#include "MillingProgramExecutor.hpp"

using namespace glm;
using namespace std;

namespace ms
{


MillingProgramExecutor::MillingProgramExecutor(
    std::shared_ptr<CuttingToolController> cuttingToolController
)
{
    _toolController = cuttingToolController;
}

MillingProgramExecutor::~MillingProgramExecutor()
{
}

void MillingProgramExecutor::setProgram(
    const std::vector<ms::PathMovement> millingProgram
)
{
    _millingProgram = millingProgram;

    if (millingProgram.size() > 1)
    {
        _toolController->setStartingPosition(millingProgram[0].position);
        _toolController->setTargetPosition(millingProgram[1].position);
        _toolController->startMovement();
        _currentProgramStep = 1;
    }
}

std::shared_ptr<CuttingToolController> MillingProgramExecutor::getController()
{
    return _toolController;
}

void MillingProgramExecutor::start()
{
    _isRunning = _currentProgramStep < _millingProgram.size();
}

void MillingProgramExecutor::stop()
{
    _isRunning = false;
}

double MillingProgramExecutor::getMoveProgress()
{
    return _toolController->getMoveProgress();
}

int MillingProgramExecutor::getExecutedStepsNum()
{
    return _currentProgramStep;
}

int MillingProgramExecutor::getTotalStepsNum()
{
    return _millingProgram.size();
}

void MillingProgramExecutor::update(double dt)
{
    if (!_isRunning) { return; }

    _toolController->update(dt);

    if (!_toolController->isMovementActive() &&
        _currentProgramStep < _millingProgram.size())
    {
        _currentProgramStep++;
        _toolController->finishMovement();

        if (_currentProgramStep < _millingProgram.size())
        {
            _toolController->setTargetPosition(
                _millingProgram[_currentProgramStep].position
            );
            _toolController->startMovement();
        }
    }
}

}
