#pragma once

#include "BsplineCurve.hpp"

#include <memory>

namespace fw
{

template <typename TPoint, typename TFloating>
class BsplineNonVanishingCurveReparametrization:
    public ICurve<TPoint, TFloating>
{
public:
    BsplineNonVanishingCurveReparametrization(
        std::shared_ptr<BsplineCurve<TPoint, TFloating>> bsplineCurve
    );

    virtual ~BsplineNonVanishingCurveReparametrization() = default;

    virtual TPoint evaluate(TFloating parameter) const override;
    virtual std::shared_ptr<ICurve<TPoint, TFloating>> getDerivativeCurve(
    ) const override;

private:
    TFloating reparametrize(TFloating parameter) const;
    std::shared_ptr<BsplineCurve<TPoint, TFloating>> _bsplineCurve;
};

using BsplineNonVanishingCurveReparametrization3d =
    BsplineNonVanishingCurveReparametrization<glm::dvec3, double>;

template<typename TPoint, typename TFloating>
BsplineNonVanishingCurveReparametrization<TPoint, TFloating>::
        BsplineNonVanishingCurveReparametrization(
    std::shared_ptr<BsplineCurve<TPoint, TFloating>> bsplineCurve
):
    _bsplineCurve{bsplineCurve}
{
}

template<typename TPoint, typename TFloating>
TPoint BsplineNonVanishingCurveReparametrization<TPoint, TFloating>::evaluate(
    TFloating parameter
) const
{
    return _bsplineCurve->evaluate(reparametrize(parameter));
}

template<typename TPoint, typename TFloating>
std::shared_ptr<ICurve<TPoint, TFloating>>
    BsplineNonVanishingCurveReparametrization<TPoint, TFloating>::
        getDerivativeCurve() const
{
    return std::static_pointer_cast<ICurve<TPoint, TFloating>>(
        std::make_shared<
            BsplineNonVanishingCurveReparametrization<TPoint, TFloating>
        >(
            _bsplineCurve->getBsplineDerivativeCurve()
        )
    );
}

template<typename TPoint, typename TFloating>
TFloating BsplineNonVanishingCurveReparametrization<TPoint, TFloating>::
    reparametrize(TFloating parameter) const
{
    const auto &knots = _bsplineCurve->getKnots();
    auto degree = _bsplineCurve->getDegree();
    auto derivativeOrder = _bsplineCurve->getDerivativeOrder();
    auto newParameter = (1 - parameter) * knots[degree+derivativeOrder]
        + parameter * knots[knots.size() - (degree + derivativeOrder) - 1];
    return newParameter;
}

}
