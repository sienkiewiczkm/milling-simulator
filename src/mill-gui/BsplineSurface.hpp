#pragma once

#include "IParametricSurfaceUV.hpp"
#include "BsplineCurve.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace fw
{
class IBsplineKnotGenerator;

enum class SurfaceFoldingMode
{
    None,
    ContinuousU,
    ContinuousV,
};

using BsplineCurve3d = BsplineCurve<glm::dvec3, double>;

class BsplineSurface:
    public IParametricSurfaceUV
{
public:
    BsplineSurface(
        int surfaceDegree,
        glm::ivec2 controlPointsGridSize,
        std::vector<glm::dvec3> controlPoints,
        std::vector<double> knotsU,
        std::vector<double> knotsV,
        SurfaceFoldingMode foldingMode = SurfaceFoldingMode::None,
        int foldDepth = 3
    );

    virtual ~BsplineSurface();

    virtual glm::dvec3 getPosition(glm::dvec2 parametrisation) const override;
    virtual glm::dvec3 getNormal(glm::dvec2 parmetrisation) const override;
    virtual glm::dvec3 getDerivativeU(
        glm::dvec2 parametrisation
    ) const override;
    virtual glm::dvec3 getDerivativeV(
        glm::dvec2 parametrisation
    ) const override;

    virtual std::shared_ptr<ICurve3d> getConstParameterCurve(
        ParametrizationAxis constParameter,
        double parameter
    ) const override;

    int getDegree() const;
    const std::vector<glm::dvec3> &getControlPoints() const;
    const std::vector<double> &getKnotsOnU() const;
    const std::vector<double> &getKnotsOnV() const;

protected:
    std::vector<glm::dvec3> evaluateConstParamControlPoints(
        int rowOrColumn,
        ParametrizationAxis constDirection
    ) const;

private:
    int _degree;
    int _foldDepth;
    glm::ivec2 _controlPointsGridSize;
    std::vector<glm::dvec3> _controlPoints;
    std::vector<double> _knotsU;
    std::vector<double> _knotsV;
    SurfaceFoldingMode _foldingMode;
};

}
