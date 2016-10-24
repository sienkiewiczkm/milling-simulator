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

std::shared_ptr<MillingBlock> MillingProgramExecutor::getMillingBlock() const
{
    return _block;
}

void MillingProgramExecutor::setMillingBlock(
    std::shared_ptr<MillingBlock> block
)
{
    _block = block;
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

MillingError MillingProgramExecutor::update(double dt)
{
    if (!_isRunning) { return MillingError::None; }

    auto timeLeft = dt;
    
    while (timeLeft > 0.001f && _currentProgramStep < _millingProgram.size())
    {
        timeLeft = _toolController->update(dt);

        auto errorState = _block->moveTool(
            _toolController->getLastPosition(),
            _toolController->getCurrentPosition(),
            _toolController->getCuttingToolParams()
        );

        if (errorState != MillingError::None)
        {
            // abort and notify if move is prohibited
            _toolController->finishMovement();
            _millingProgram.clear();
            _isRunning = false;
            return errorState;
        }

        if (!_toolController->isMovementActive())
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

    return MillingError::None;
}

}
