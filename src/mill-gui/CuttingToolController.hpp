#pragma once

#include <glm/glm.hpp>

namespace ms
{
class CuttingToolController
{
public:
    CuttingToolController();
    ~CuttingToolController();

    glm::vec3 getStartingPosition();
    void setStartingPosition(const glm::vec3 &position);
    glm::vec3 getLastPosition();
    glm::vec3 getCurrentPosition();
    glm::vec3 getTargetPosition();
    void setTargetPosition(const glm::vec3 &position);
    void setMovementSpeed(float speed);

    void startMovement();
    void finishMovement();

    bool isMovementActive();
    
    void update(float dt);

private:
    bool _moving;

    float _speed;
    float _distanceMoved;
    float _totalDistance;

    glm::vec3 _startingPosition;
    glm::vec3 _lastPosition;
    glm::vec3 _currentPosition;
    glm::vec3 _targetPosition;
};
}
