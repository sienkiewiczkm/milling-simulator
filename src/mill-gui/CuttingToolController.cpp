#include "CuttingToolController.hpp"

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
    _startingPosition = position;
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

void CuttingToolController::setMovementSpeed(double speed)
{
    _speed = speed;
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

void CuttingToolController::update(double dt)
{
    _distanceMoved += dt * _speed;
    _lastPosition = _currentPosition;

    if (_distanceMoved >= _totalDistance)
    {
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
}

}
