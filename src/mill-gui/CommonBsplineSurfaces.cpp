#include "CommonBsplineSurfaces.hpp"
#include "BsplineEquidistantKnotGenerator.hpp"

namespace fw
{

std::shared_ptr<BsplineSurface> createBsplinePlane(
    glm::dvec3 uv0,
    glm::dvec3 u1,
    glm::dvec3 v1,
    glm::dvec3 uv1,
    glm::dmat4 controlPointTransformation,
    glm::ivec2 controlPointsGridSize
)
{
    BsplineEquidistantKnotGenerator knotGenerator;

    std::vector<glm::dvec3> controlPointsGrid(
        controlPointsGridSize.x * controlPointsGridSize.y
    );

    for (auto y = 0; y < controlPointsGridSize.y; ++y)
    {
        auto dy = y / static_cast<double>(controlPointsGridSize.y - 1);
        auto left = glm::mix(uv0, v1, dy);
        auto right = glm::mix(u1, uv1, dy);

        for (auto x = 0; x < controlPointsGridSize.x; ++x)
        {
            auto dx = x / static_cast<double>(controlPointsGridSize.x - 1);
            auto point = glm::mix(left, right, dx);
            controlPointsGrid[controlPointsGridSize.y * y + x] =
                glm::dvec3(controlPointTransformation * glm::dvec4(point, 1.0));
        }
    }

    int degree = 3;
    return std::make_shared<BsplineSurface>(
        degree,
        controlPointsGridSize,
        controlPointsGrid,
        knotGenerator.generate(controlPointsGridSize.x, degree),
        knotGenerator.generate(controlPointsGridSize.y, degree)
    );
}

}
