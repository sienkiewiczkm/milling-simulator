#pragma once

#include "IBasisEvaluator.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace fw
{

template <typename TPoint, typename TFloating = double>
class LinearCombinationEvaluator
{
public:
    LinearCombinationEvaluator(
        std::shared_ptr<IBasisEvaluator> basisEvaluator,
        std::vector<TPoint> controlPoints
    ):
        _basisEvaluator{basisEvaluator},
        _controlPoints{controlPoints}
    {
    }

    virtual ~LinearCombinationEvaluator()
    {
    }

    TPoint evaluate(TFloating parameter) const
    {
        auto basis = _basisEvaluator->evaluate(parameter);

        assert(basis.size() > 0);
        assert(basis.size() >= _controlPoints.size());

        TPoint output = basis[0] * _controlPoints[0];
        for (auto i = 1; i < _controlPoints.size(); ++i)
        {
            output += basis[i] * _controlPoints[i];
        }

        return output;
    }

private:
    std::shared_ptr<IBasisEvaluator> _basisEvaluator;
    std::vector<TPoint> _controlPoints;
};

}
