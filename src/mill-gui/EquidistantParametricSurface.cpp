#include "EquidistantParametricSurface.hpp"

namespace fw
{

EquidistantParametricSurfaceCurve::EquidistantParametricSurfaceCurve(
    std::shared_ptr<const EquidistantParametricSurface> referenceSurface,
    ParametrizationAxis constantAxis,
    double constantParameter
):
    _referenceSurface{referenceSurface},
    _constantAxis{constantAxis},
    _constantParameter{constantParameter}
{
}

glm::dvec3 EquidistantParametricSurfaceCurve::evaluate(double parameter) const
{
    auto parameterization = _constantAxis == ParametrizationAxis::U
        ? glm::dvec2{_constantParameter, parameter}
        : glm::dvec2{parameter, _constantParameter};
    return _referenceSurface->getPosition(parameterization);
}

std::shared_ptr<ICurve3d> EquidistantParametricSurfaceCurve::getDerivativeCurve(
) const
{
    return _referenceSurface
        ->getReferenceSurface()
        ->getConstParameterCurve(_constantAxis, _constantParameter)
        ->getDerivativeCurve();
}

EquidistantParametricSurface::EquidistantParametricSurface(
    const std::shared_ptr<fw::IParametricSurfaceUV>& referenceSurface,
    double normalDistance
):
    _referenceSurface{referenceSurface},
    _normalDistance{normalDistance}
{
}

EquidistantParametricSurface::~EquidistantParametricSurface()
{
}

std::shared_ptr<fw::IParametricSurfaceUV>
        EquidistantParametricSurface::getReferenceSurface() const
{
    return _referenceSurface;
}

std::shared_ptr<ICurve3d> EquidistantParametricSurface::getConstParameterCurve(
    ParametrizationAxis constParameter,
    double parameter
) const
{
    return std::static_pointer_cast<ICurve3d>(
        std::make_shared<EquidistantParametricSurfaceCurve>(
            shared_from_this(),
            constParameter,
            parameter
        )
    );
}

glm::dvec3 EquidistantParametricSurface::getPosition(
    glm::dvec2 parametrisation
) const
{
    const auto& originalPosition = _referenceSurface->getPosition(
        parametrisation
    );

    const auto& normal = getNormal(parametrisation);

    return originalPosition + _normalDistance * normal;
}

glm::dvec3 EquidistantParametricSurface::getNormal(
    glm::dvec2 parametrisation
) const
{
    return glm::normalize(_referenceSurface->getNormal(parametrisation));
}

glm::dvec3 EquidistantParametricSurface::getDerivativeU(
    glm::dvec2 parameterization
) const
{
    // todo: verify if those derivatives are correct
    return _referenceSurface->getDerivativeU(parameterization);
}

glm::dvec3 EquidistantParametricSurface::getDerivativeV(
    glm::dvec2 parameterization
) const
{
    // todo: verify if those derivatives are correct
    return _referenceSurface->getDerivativeV(parameterization);
}

}
