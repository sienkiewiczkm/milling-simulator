#include "MillingProgramExecutor.hpp"

using namespace glm;
using namespace std;

namespace ms
{


MillingProgramExecutor::MillingProgramExecutor(
    std::shared_ptr<CuttingToolController> cuttingToolController
):
    _isRunning(false)
{
    _toolController = cuttingToolController;
}

MillingProgramExecutor::~MillingProgramExecutor()
{
}

const std::string &MillingProgramExecutor::getProgramName() const
{
    return _programName;
}

void MillingProgramExecutor::setProgram(
    const std::string &programName,
    const std::vector<ms::PathMovement> &millingProgram
)
{
    if (millingProgram.size() == 0)
    {
        throw new string("Milling program must be non-empty");
    }

    _programName = programName;
    _millingProgram = millingProgram;

    if (millingProgram.size() > 1)
    {
        _toolController->setTargetPosition(millingProgram[0].position);
        _toolController->startMovement();
        _currentProgramStep = 0;
    }

    stop();
}

bool MillingProgramExecutor::isProgramLoaded()
{
    return _millingProgram.size() > 0;
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
