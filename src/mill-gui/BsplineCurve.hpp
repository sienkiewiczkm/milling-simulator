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
        std::vector<TFloating> knots
    );
    virtual ~BsplineCurve() = default;

    virtual TPoint evaluate(TFloating parameter) override;

private:
    int _degree;
    std::vector<TPoint> _controlPoints;
    std::vector<TFloating> _knots;
    std::shared_ptr<BsplineBasisEvaluator> _basisEvaluator;
    std::shared_ptr<LinearCombinationEvaluator<TPoint, TFloating>>
        _linearCombination;
};

using BsplineCurve2f = BsplineCurve<glm::vec2, float>;
using BsplineCurve2d = BsplineCurve<glm::dvec2, double>;
using BsplineCurve3f = BsplineCurve<glm::vec3, float>;
using BsplineCurve3d = BsplineCurve<glm::dvec3, double>;

template<typename TPoint, typename TFloating>
BsplineCurve<TPoint, TFloating>::BsplineCurve(
    int degree,
    std::vector<TPoint> controlPoints,
    std::vector<TFloating> knots
):
    _degree{degree},
    _controlPoints{controlPoints},
    _knots{knots}
{
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
TPoint BsplineCurve<TPoint, TFloating>::evaluate(TFloating parameter)
{
    return _linearCombination->evaluate(parameter);
}

}
