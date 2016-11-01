#include "BsplineBasisEvaluator.hpp"
#include <limits>
#include <cmath>

namespace fw
{

BsplineBasisEvaluator::BsplineBasisEvaluator(
    int degree,
    const std::vector<double> &knots
):
    _degree{degree},
    _knots{knots}
{
}

BsplineBasisEvaluator::~BsplineBasisEvaluator()
{
}

std::vector<double> BsplineBasisEvaluator::evaluate(
    double parameter
) const
{
    auto basis = evaluateZeroDegreeBasis(parameter);

    for (auto currentDegree = 0; currentDegree < _degree; ++currentDegree)
    {
        increaseBasisDegree(basis, currentDegree, parameter);
    }

    // discard temporary values not needed after degree reduction
    basis.resize(_knots.size() - _degree - 1);

    return basis;
}

std::vector<double> BsplineBasisEvaluator::evaluateZeroDegreeBasis(
    double parameter
) const
{
    std::vector<double> basis(_knots.size() - 1);
    for (auto i = 0; i < _knots.size() - 1; ++i)
    {
        double currentInternalValue = 0.0;
        if (_knots[i] <= parameter && parameter < _knots[i+1])
        {
            currentInternalValue = 1.0;
        }
        basis[i] = currentInternalValue;
    }
    return basis;
}

void BsplineBasisEvaluator::increaseBasisDegree(
    std::vector<double> &basis,
    int currentDegreeOfBasis,
    double parameter
) const
{
    auto targetDegree = currentDegreeOfBasis + 1;
    for (auto i = 0; i < _knots.size() - targetDegree - 1; ++i)
    {
        auto leftNumerator = parameter - _knots[i];
        auto leftDenominator = _knots[i + targetDegree] - _knots[i];

        auto rightNumerator = _knots[i + targetDegree + 1] - parameter;
        auto rightDenominator = _knots[i + targetDegree + 1] - _knots[i + 1];

        auto leftCoefficient =
            std::abs(leftDenominator) > std::numeric_limits<double>::epsilon()
            ? leftNumerator / leftDenominator
            : 0.0;

        auto rightCoefficient =
            std::abs(rightDenominator) > std::numeric_limits<double>::epsilon()
            ? rightNumerator / rightDenominator
            : 0.0;

        auto currentIntervalValue = leftCoefficient * basis[i]
            + rightCoefficient * basis[i + 1];

        basis[i] = currentIntervalValue;
    }
}

}
