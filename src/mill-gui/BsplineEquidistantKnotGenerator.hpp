#pragma once

#include <vector>

namespace fw
{

class IBsplineKnotGenerator
{
public:
    virtual ~IBsplineKnotGenerator() = default;
    virtual std::vector<double> generate(
        int controlPointsAmount,
        int degree
    ) const = 0;
};

class BsplineEquidistantKnotGenerator:
    public IBsplineKnotGenerator
{
public:
    virtual std::vector<double> generate(
        int controlPointsAmount,
        int degree
    ) const;
};

}
