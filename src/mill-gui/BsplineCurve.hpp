#pragma once

#include "ICurve.hpp"
#include "BsplineBasisEvaluator.hpp"
#include "LinearCombinationEvaluator.hpp"

#include <vector>

#include <glm/glm.hpp>

namespace fw
{

template<typename TPoint, typename TFloating>
class BsplineCurve:
    public ICurve<TPoint, TFloating>
{
public:
    BsplineCurve(
        int degree,
        std::vector<TPoint> controlPoints,
        std::vector<TFloating> knots,
        int derivativeOrder = 0
    );
    virtual ~BsplineCurve() = default;

    int getDegree() const { return _degree; }
    int getDerivativeOrder() const { return _derivativeOrder; }
    virtual const std::vector<TPoint> &getControlPoints() const;
    virtual const std::vector<TFloating> &getKnots() const;

    virtual TPoint evaluate(TFloating parameter) const override;

    virtual std::shared_ptr<ICurve<TPoint, TFloating>> getDerivativeCurve(
    ) const override;

    virtual std::shared_ptr<BsplineCurve<TPoint, TFloating>>
            getBsplineDerivativeCurve(
    ) const;

private:
    int _degree;
    int _derivativeOrder;
    std::vector<TPoint> _controlPoints;
    std::vector<TFloating> _knots;
    std::shared_ptr<BsplineBasisEvaluator> _basisEvaluator;
    std::shared_ptr<LinearCombinationEvaluator<TPoint, TFloating>>
        _linearCombination;
    mutable std::shared_ptr<BsplineCurve<TPoint, TFloating>> _derivativeCurve;
};

using BsplineCurve2f = BsplineCurve<glm::vec2, float>;
using BsplineCurve2d = BsplineCurve<glm::dvec2, double>;
using BsplineCurve3f = BsplineCurve<glm::vec3, float>;
using BsplineCurve3d = BsplineCurve<glm::dvec3, double>;

template<typename TPoint, typename TFloating>
BsplineCurve<TPoint, TFloating>::BsplineCurve(
    int degree,
    std::vector<TPoint> controlPoints,
    std::vector<TFloating> knots,
    int derivativeOrder
):
    _degree{degree},
    _derivativeOrder{derivativeOrder},
    _controlPoints(_derivativeOrder + controlPoints.size()),
    _knots{knots}
{
    for (auto i = 0; i < controlPoints.size(); ++i)
    {
        _controlPoints[_derivativeOrder + i] = controlPoints[i];
    }

    _basisEvaluator = std::make_shared<BsplineBasisEvaluator>(
        _degree,
        _knots
    );

    _linearCombination =
        std::make_shared<LinearCombinationEvaluator<TPoint, TFloating>>(
            _basisEvaluator,
            _controlPoints
        );
}

template<typename TPoint, typename TFloating>
TPoint BsplineCurve<TPoint, TFloating>::evaluate(TFloating parameter) const
{
    return _linearCombination->evaluate(parameter);
}

template<typename TPoint, typename TFloating>
const std::vector<TPoint> &BsplineCurve<TPoint, TFloating>::getControlPoints(
) const
{
    return _controlPoints;
}

template<typename TPoint, typename TFloating>
const std::vector<TFloating> &BsplineCurve<TPoint, TFloating>::getKnots(
) const
{
    return _knots;
}

template<typename TPoint, typename TFloating>
std::shared_ptr<ICurve<TPoint, TFloating>>
        BsplineCurve<TPoint, TFloating>::getDerivativeCurve() const
{
    return std::static_pointer_cast<ICurve<TPoint, TFloating>>(
        getBsplineDerivativeCurve()
    );
}

template<typename TPoint, typename TFloating>
std::shared_ptr<BsplineCurve<TPoint, TFloating>>
        BsplineCurve<TPoint, TFloating>::getBsplineDerivativeCurve(
) const
{
    if (_derivativeCurve == nullptr)
    {
        std::vector<TPoint> derivativeControlPoints(_controlPoints.size() - 1);
        for (auto i = 0; i < _controlPoints.size() - 1; ++i)
        {
            auto coefficient = (_degree) / (_knots[i+_degree+1] - _knots[i+1]);
            auto newControlPoint =
                coefficient * (_controlPoints[i+1] - _controlPoints[i]);

            derivativeControlPoints[i] = newControlPoint;
        }

        _derivativeCurve = std::make_shared<BsplineCurve<TPoint, TFloating>>(
            _degree - 1,
            derivativeControlPoints,
            _knots,
            _derivativeOrder + 1
        );
    }

    return _derivativeCurve;
}

}
