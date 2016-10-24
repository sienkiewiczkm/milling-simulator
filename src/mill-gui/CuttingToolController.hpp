#pragma once

#include "CuttingToolParams.hpp"

#include <glm/glm.hpp>

namespace ms
{

/** \brief This controller is in charge of moving the tool as smooth as possible
 *         on linear paths.
 */
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
    double getMovementSpeed();
    void setMovementSpeed(double speed);

    double getMoveProgress() const;

    const CuttingToolParams &getCuttingToolParams() const;
    void setCuttingToolParams(const CuttingToolParams &params);

    void startMovement();
    void finishMovement();

    bool isMovementActive();
    
    double update(double dt);
    void fastForward();

private:
    bool _moving;

    CuttingToolParams _cuttingToolParams;

    double _speed;
    double _distanceMoved;
    double _totalDistance;

    glm::dvec3 _startingPosition;
    glm::dvec3 _lastPosition;
    glm::dvec3 _currentPosition;
    glm::dvec3 _targetPosition;
};

}
