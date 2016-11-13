#pragma once

#include "glm/glm.hpp"
#include <vector>

namespace ms
{

class ContourMerger
{
public:
    ContourMerger();
    ~ContourMerger();

    std::vector<glm::dvec3> merge2D(
        const std::vector<glm::dvec3> &lhs,
        const std::vector<glm::dvec3> &rhs
    ) const;
};

}
