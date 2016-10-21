#include "Common.hpp"

#include <cmath>
#include <fstream>
#include <streambuf>

using namespace std;

double common::pi()
{
    return std::acos(-1.0);
}

float common::pif()
{
    return std::acos(-1.0f);
}

std::string common::loadASCIITextFile(const std::string &filepath)
{
    ifstream file(filepath);
    string output;

    file.seekg(0, ios::end);
    output.reserve(file.tellg());
    file.seekg(0, ios::beg);

    output.assign(
        istreambuf_iterator<char>(file),
        istreambuf_iterator<char>()
    );

    return output;
}

const char *common::transformStringToCStr(const std::string &str)
{
    return str.c_str();
}
