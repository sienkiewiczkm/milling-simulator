#pragma once

#include <cmath>
#include <string>

namespace common
{
    double pi();
    float pif();

    std::string loadASCIITextFile(const std::string &filepath);

    const char *transformStringToCStr(const std::string &str);
}
