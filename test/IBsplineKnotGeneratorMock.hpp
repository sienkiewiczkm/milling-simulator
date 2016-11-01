#pragma once

#include <gmock/gmock.h>
#include "BsplineEquidistantKnotGenerator.hpp"

class IBsplineKnotGeneratorMock:
    public fw::IBsplineKnotGenerator
{
public:
    MOCK_CONST_METHOD2(generate, std::vector<double>(
        int controlPointsAmount,
        int degree
    ));
};
