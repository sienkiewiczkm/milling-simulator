#pragma once

#include "MillPathFormatReader.hpp"
#include "glm/glm.hpp"
#include <vector>

namespace ms
{

class BaseFlatteningPathGenerator
{
public:
    BaseFlatteningPathGenerator();
    ~BaseFlatteningPathGenerator();

    void setBaseHeight(double baseHeight);
    void setCuttingToolRadius(double radius);
    void setWorkingArea(glm::dvec3 blockSize, glm::dmat4 worldMatrix);
    void setObjectContours(const std::vector<glm::dvec3>& objectContour);

    void bake();
    std::vector<PathMovement> buildPaths();

protected:
    void createBorderEqualizationPath();
    glm::dvec3 findSafeEntryPoint(const glm::dvec3& pos) const;

private:
    std::vector<glm::dvec3> _objectContour;
    double _toolRadius;
    double _baseHeight;
    double _safeHeight;
    glm::dvec3 _blockSize;
    glm::dmat4 _blockWorldMatrix;
    glm::dmat4 _blockWorldMatrixInv;
    std::vector<std::vector<glm::dvec3>> _paths;
};

}
