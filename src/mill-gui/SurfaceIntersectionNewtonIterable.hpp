#pragma once
#include "NewtonIterator.hpp"
#include "IParametricSurfaceUV.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace fw
{

using SurfaceIntersectionNewtonIterator =
    NewtonIterator<glm::dvec4, glm::dvec4, glm::dmat4>;

class SurfaceIntersectionNewtonIterable:
    public INewtonIterable<glm::dvec4, glm::dvec4, glm::dmat4>
{
public:
    SurfaceIntersectionNewtonIterable();

    void setSurfaces(
        const std::shared_ptr<IParametricSurfaceUV> lhs,
        const std::shared_ptr<IParametricSurfaceUV> rhs
    );

    void setTangentVector(const glm::dvec3& tangentVector);
    void setPlaneDistance(double planeDistance);

    double getCovergenceThreshold() const;
    void setCovergenceThreshold(double threshold);

    virtual void setInitialParameters(const glm::dvec4& parameters) override;
    virtual void setCurrentParameters(const glm::dvec4& parameters) override;
    virtual bool areParametersValid(
        const glm::dvec4& parameters
    ) const override;
    virtual bool hasParameterConverged(
        const glm::dvec4& newParameters
    ) const override;
    virtual glm::dvec4 evaluateFunction() const override;
    virtual glm::dmat4 getJacobianInverse() const override;
    virtual glm::dvec4 correctParametrisation(
        const glm::dvec4& parameters
    ) const override;

private:
    double calculateTangentPlaneDistance(const glm::dvec3& lhsPosition) const;
    glm::dvec4 getTangentPlaneDerivatives() const;
    double calculateTangentPlaneSingleDerivative(
        const glm::dvec2 &lhsParams,
        const glm::dvec2& direction,
        const double delta
    ) const;

    bool hasSingleParameterConverged(
        const glm::dvec2& previous,
        const glm::dvec2& current
    ) const;

    double _planeDistance;
    double _covergenceThreshold;
    glm::dvec4 _initialParameters;
    glm::dvec4 _parameters;
    glm::dvec3 _initialLhsPosition;
    glm::dvec3 _tangentVector;
    std::shared_ptr<IParametricSurfaceUV> _lhsSurface;
    std::shared_ptr<IParametricSurfaceUV> _rhsSurface;
};

}
