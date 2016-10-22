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
    CuttingToolKind kind;
    double radius;
};

}
