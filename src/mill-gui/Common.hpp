#pragma once

#include <cmath>
#include <string>

namespace common
{
    double pi();
    float pif();

    std::string loadASCIITextFile(const std::string &filepath);

    const char *transformStringToCStr(const std::string &str);

    template <typename T>
    int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }
}
