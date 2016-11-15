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

    std::vector<glm::dvec2> merge2D(
        const std::vector<glm::dvec2> &lhs,
        const std::vector<glm::dvec2> &rhs
    ) const;

    std::vector<glm::dvec3> merge2D(
        const std::vector<glm::dvec3> &lhs,
        const std::vector<glm::dvec3> &rhs
    ) const;

private:
    void followContourAdding(
        const std::vector<glm::dvec3> &lhs,
        std::vector<glm::dvec3>& output,
        int from,
        int to,
        int increase = 1
    ) const;
};

}
