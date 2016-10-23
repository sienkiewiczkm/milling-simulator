#pragma once

namespace ms
{

enum class CuttingToolKind
{
    Unknown,
    Ball,
    Flat
};

struct CuttingToolParams
{
    CuttingToolParams();
    CuttingToolParams(CuttingToolKind kind, double radius);

    CuttingToolKind kind;
    double radius;
};

}
