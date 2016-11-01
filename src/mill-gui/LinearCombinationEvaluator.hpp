#pragma once

#include "IBasisEvaluator.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace fw
{

template <typename T>
class LinearCombinationEvaluator
{
public:
    LinearCombinationEvaluator(
        std::shared_ptr<IBasisEvaluator> basisEvaluator,
        std::vector<T> controlPoints
    ):
        _basisEvaluator{basisEvaluator},
        _controlPoints{controlPoints}
    {
    }

    virtual ~LinearCombinationEvaluator()
    {
    }

    T evaluate(double parameter) const
    {
        auto basis = _basisEvaluator->evaluate(parameter);

        assert(basis.size() > 0);
        assert(basis.size() == _controlPoints.size());

        T output = basis[0] * _controlPoints[0];
        for (auto i = 1; i < basis.size(); ++i)
        {
            output += basis[i] * _controlPoints[i];
        }

        return output;
    }

private:
    std::shared_ptr<IBasisEvaluator> _basisEvaluator;
    std::vector<T> _controlPoints;
};

}
