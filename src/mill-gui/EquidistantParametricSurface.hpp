#pragma once

#include "ICurve.hpp"
#include "IParametricSurfaceUV.hpp"

#include <memory>

namespace fw
{

class EquidistantParametricSurface;

class EquidistantParametricSurfaceCurve:
    public ICurve3d
{
public:
    EquidistantParametricSurfaceCurve(
        std::shared_ptr<const EquidistantParametricSurface> referenceSurface,
        ParametrizationAxis constantAxis,
        double constantParameter
    );

    virtual glm::dvec3 evaluate(double parameter) const override;
    virtual std::shared_ptr<ICurve3d> getDerivativeCurve() const override;

private:
    ParametrizationAxis _constantAxis;
    double _constantParameter;
    std::shared_ptr<const EquidistantParametricSurface> _referenceSurface;
};

class EquidistantParametricSurface:
    public IParametricSurfaceUV,
    public std::enable_shared_from_this<EquidistantParametricSurface>
{
public:
    EquidistantParametricSurface(
        const std::shared_ptr<fw::IParametricSurfaceUV>& referenceSurface,
        double normalDistance = 1.0
    );
    ~EquidistantParametricSurface();

    std::shared_ptr<fw::IParametricSurfaceUV> getReferenceSurface() const;
    inline void setNormalDistance(double distance) {_normalDistance = distance;}
    inline double getNormalDistance() const { return _normalDistance; }

    virtual std::shared_ptr<ICurve3d> getConstParameterCurve(
        ParametrizationAxis constParameter,
        double parameter
    ) const override;

    virtual glm::dvec3 getPosition(glm::dvec2 parametrisation) const override;
    virtual glm::dvec3 getNormal(glm::dvec2 parmetrisation) const override;
    virtual glm::dvec3 getDerivativeU(
        glm::dvec2 parametrisation
    ) const override;
    virtual glm::dvec3 getDerivativeV(
        glm::dvec2 parametrisation
    ) const override;

private:
    std::shared_ptr<fw::IParametricSurfaceUV> _referenceSurface;
    double _normalDistance;
};

}
