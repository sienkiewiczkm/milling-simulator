#include "BsplineEquidistantKnotGenerator.hpp"

namespace fw
{

std::vector<double> BsplineEquidistantKnotGenerator::generate(
    int controlPointsAmount,
    int degree
) const
{
    std::vector<double> output;
    auto knotsAmount = controlPointsAmount + degree + 1;
    output.resize(knotsAmount);

    for (auto i = 0; i < knotsAmount; ++i)
    {
        output[i] = static_cast<double>(i)/(knotsAmount - 1);
    }

    return output;
}

}
