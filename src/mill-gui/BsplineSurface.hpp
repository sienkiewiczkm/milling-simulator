#pragma once

#include "LinearCombinationEvaluator.hpp"
#include "IParametricSurfaceUV.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace fw
{
class IBsplineKnotGenerator;

/** \brief BsplinePatch with maximum possible degree on edge.
 *
 */
class BsplineSurface:
    public IParametricSurfaceUV
{
public:
    BsplineSurface(
        int surfaceDegree,
        glm::ivec2 controlPointsGridSize,
        std::vector<glm::dvec3> controlPoints,
        std::shared_ptr<IBsplineKnotGenerator> knotGenerator
    );

    virtual ~BsplineSurface();

    virtual glm::dvec3 getPosition(glm::dvec2 parametrisation);
    virtual glm::dvec3 getNormal(glm::dvec2 parmetrisation);

    int getDegree() const;
    const std::vector<glm::dvec3> &getControlPoints() const;
    const std::vector<double> &getKnotsOnU() const;
    const std::vector<double> &getKnotsOnV() const;

protected:
    enum class EvaluationDirection
    {
        AlongXAxis,
        AlongYAxis
    };

    std::vector<glm::dvec3> evaluateSubcontrolPoints(
        int rowOrColumn,
        EvaluationDirection direction
    ) const;

    glm::dvec3 evaluateCurve(
        const std::vector<glm::dvec3> &controlPoints,
        double parameter
    );

private:
    int _degree;
    glm::ivec2 _controlPointsGridSize;
    std::vector<glm::dvec3> _controlPoints;
    std::shared_ptr<IBsplineKnotGenerator> _knotGenerator;
    std::vector<double> _knotsX;
    std::vector<double> _knotsY;
};

}
