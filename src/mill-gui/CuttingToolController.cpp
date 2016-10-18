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

glm::vec3 CuttingToolController::getStartingPosition()
{
    return _startingPosition;
}

void CuttingToolController::setStartingPosition(const glm::vec3 &position)
{
    _startingPosition = position;
}

glm::vec3 CuttingToolController::getLastPosition()
{
    return _lastPosition;
}

glm::vec3 CuttingToolController::getCurrentPosition()
{
    return _currentPosition;
}

glm::vec3 CuttingToolController::getTargetPosition()
{
    return _targetPosition;
}

void CuttingToolController::setTargetPosition(const glm::vec3 &position)
{
    _targetPosition = position;
}

void CuttingToolController::setMovementSpeed(float speed)
{
    _speed = speed;
}

void CuttingToolController::startMovement()
{
    if (_moving) { return; /* todo: react with an error */}

    _distanceMoved = 0.0f;
    _totalDistance = glm::length(_targetPosition - _startingPosition);
    _moving = true;
}

void CuttingToolController::finishMovement()
{
    _startingPosition = _currentPosition;
    _distanceMoved = 0.0f;
    _moving = false;
}

bool CuttingToolController::isMovementActive()
{
    return _moving;
}

void CuttingToolController::update(float dt)
{
    _distanceMoved += dt * _speed;

    if (_distanceMoved >= _totalDistance)
    {
        _distanceMoved = _totalDistance;
        _moving = false;
    }

    _lastPosition = _currentPosition;

    float alpha = _distanceMoved / _totalDistance;
    _currentPosition = (1.0f - alpha) * _startingPosition 
        + alpha * _targetPosition;
}

}
