#pragma once

#include <glm/glm.hpp>

namespace fw
{

template<typename TPoint, typename TFloating>
class ICurve
{
public:
    virtual ~ICurve() = default;
    virtual TPoint evaluate(TFloating parameter) = 0;
};

using ICurve3f = ICurve<glm::vec3, float>;
using ICurve3d = ICurve<glm::dvec3, double>;

}
