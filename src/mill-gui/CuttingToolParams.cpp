#include "CuttingToolParams.hpp"

namespace ms
{

CuttingToolParams::CuttingToolParams():
    kind(CuttingToolKind::Ball),
    radius(10.0f)
{
}

CuttingToolParams::CuttingToolParams(CuttingToolKind kind, double radius):
    kind(kind),
    radius(radius)
{
}

}
