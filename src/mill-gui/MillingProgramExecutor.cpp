#include "MillingProgramExecutor.hpp"
#include <chrono>

using namespace glm;
using namespace std;

namespace ms
{

MillingProgramExecutor::MillingProgramExecutor(
    std::shared_ptr<CuttingToolController> cuttingToolController
):
    _isRunning(false),
    _fastForwardMode(false),
    _currentProgramStep(0)
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

void MillingProgramExecutor::enableFastForward()
{
    _fastForwardMode = true;
}

bool MillingProgramExecutor::isInFastForwardMode()
{
    if (!_isRunning) { _fastForwardMode = false; }
    return _fastForwardMode;
}

MillingError MillingProgramExecutor::update(double dt)
{
    if (!_isRunning) { return MillingError::None; }

    auto startTime = std::chrono::high_resolution_clock::now();
    auto maxDuration = std::chrono::milliseconds(50);

    auto timeLeft = dt;
    while (timeLeft > 0.001 && _currentProgramStep < _millingProgram.size())
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        if (currentTime - startTime > maxDuration)
        {
            break;
        }

        if (_fastForwardMode)
        {
            _toolController->fastForward();
            timeLeft = 0.0;
        }
        else
        {
            timeLeft = _toolController->update(dt);
        }

        bool isCollisionProhibited = _millingProgram[_currentProgramStep].type 
            == PathMovementType::FastMovement;

        auto errorState = _block->moveTool(
            _toolController->getLastPosition(),
            _toolController->getCurrentPosition(),
            _toolController->getCuttingToolParams(),
            isCollisionProhibited
        );

        if (errorState != MillingError::None)
        {
            // abort and notify if move is prohibited
            _toolController->abortMovement();
            _millingProgram.clear();

            _isRunning = false;
            _fastForwardMode = false;

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
            else
            {
            }
        }
    }

    if (_currentProgramStep >= _millingProgram.size())
    {
        _isRunning = false;
        _fastForwardMode = false;
    }

    return MillingError::None;
}

}
