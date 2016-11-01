#pragma once

#include "IBasisEvaluator.hpp"
#include <vector>

namespace fw
{

class BsplineBasisEvaluator:
    public IBasisEvaluator
{
public:
    BsplineBasisEvaluator(
        int degree,
        const std::vector<double> &knots
    );

    virtual ~BsplineBasisEvaluator(); 

    virtual std::vector<double> evaluate(
        double parameter
    ) const override;

protected:
    const int _degree;
    const std::vector<double> _knots;

    std::vector<double> evaluateZeroDegreeBasis(
        double parameter
    ) const;

    void increaseBasisDegree(
        std::vector<double> &basis,
        int currentDegreeOfBasis,
        double paramter
    ) const;
};

}
