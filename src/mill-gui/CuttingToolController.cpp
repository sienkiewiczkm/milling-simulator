#include "CuttingToolController.hpp"

#include <algorithm>
#include <iostream>

using namespace std;

namespace ms {

CuttingToolController::CuttingToolController() :
    _moving(false),
    _speed(1.0f),
    _distanceMoved(0.0f),
    _totalDistance(0.0f)
{ 
}

CuttingToolController::~CuttingToolController()
{
}

glm::dvec3 CuttingToolController::getStartingPosition()
{
    return _startingPosition;
}

void CuttingToolController::setStartingPosition(const glm::dvec3 &position)
{
    _lastPosition = _currentPosition = _startingPosition = position;
}

glm::dvec3 CuttingToolController::getLastPosition()
{
    return _lastPosition;
}

glm::dvec3 CuttingToolController::getCurrentPosition()
{
    return _currentPosition;
}

glm::dvec3 CuttingToolController::getTargetPosition()
{
    return _targetPosition;
}

void CuttingToolController::setTargetPosition(const glm::dvec3 &position)
{
    _targetPosition = position;
}

double CuttingToolController::getMovementSpeed()
{
    return _speed;
}

void CuttingToolController::setMovementSpeed(double speed)
{
    _speed = speed;
}

double CuttingToolController::getMoveProgress() const
{
    return std::min(_distanceMoved / _totalDistance, 1.0);
}

const CuttingToolParams &CuttingToolController::getCuttingToolParams() const
{
    return _cuttingToolParams;
}

void CuttingToolController::setCuttingToolParams(
    const CuttingToolParams &params
)
{
   _cuttingToolParams = params; 
}

void CuttingToolController::startMovement()
{
    if (_moving) { return; /* todo: react with an error */}

    _distanceMoved = 0.0;
    _totalDistance = glm::length(_targetPosition - _startingPosition);
    _moving = true;
}

void CuttingToolController::finishMovement()
{
    _startingPosition = _targetPosition;
    _distanceMoved = 0.0;
    _moving = false;
}

bool CuttingToolController::isMovementActive()
{
    return _moving;
}

double CuttingToolController::update(double dt)
{
    auto prevDistanceMoved = _distanceMoved;
    _distanceMoved += dt * _speed;

    _lastPosition = _currentPosition;

    double timeLeft = 0.0;

    if (_distanceMoved >= _totalDistance)
    {
        auto timeNeeded = std::max(
            0.05,
            (_totalDistance - prevDistanceMoved)/_speed
        );

        timeLeft = std::max(0.0, dt - timeNeeded);
        
        _distanceMoved = _totalDistance;

        _moving = false;
        _currentPosition = _targetPosition;
    }
    else
    {
        auto alpha = _distanceMoved / _totalDistance;
        _currentPosition = (1.0 - alpha) * _startingPosition 
            + alpha * _targetPosition;
    }
    
    return timeLeft;
}

}
