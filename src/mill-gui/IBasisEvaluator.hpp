#pragma once

#include <vector>

namespace fw
{

class IBasisEvaluator
{
public:
    virtual ~IBasisEvaluator() = default;

    virtual std::vector<double> evaluate(
        double parameter
    ) const = 0;
};

}
