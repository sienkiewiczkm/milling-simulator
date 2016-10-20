#pragma once

#include <glm/glm.hpp>

namespace ms
{
class CuttingToolController
{
public:
    CuttingToolController();
    ~CuttingToolController();

    glm::dvec3 getStartingPosition();
    void setStartingPosition(const glm::dvec3 &position);
    glm::dvec3 getLastPosition();
    glm::dvec3 getCurrentPosition();
    glm::dvec3 getTargetPosition();
    void setTargetPosition(const glm::dvec3 &position);
    void setMovementSpeed(double speed);

    void startMovement();
    void finishMovement();

    bool isMovementActive();
    
    void update(double dt);

private:
    bool _moving;

    double _speed;
    double _distanceMoved;
    double _totalDistance;

    glm::dvec3 _startingPosition;
    glm::dvec3 _lastPosition;
    glm::dvec3 _currentPosition;
    glm::dvec3 _targetPosition;
};
}
