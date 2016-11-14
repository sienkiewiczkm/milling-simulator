#pragma once

#include "MillPathFormatReader.hpp"
#include "glm/glm.hpp"
#include <vector>

namespace ms
{

enum class ZigZagRejectionMode
{
    Unknown,
    Inside,
    Outside
};

class ZigZagPathGenerator
{
public:
    ZigZagPathGenerator();
    ~ZigZagPathGenerator();

    void setRejectionMode(ZigZagRejectionMode rejectionMode);
    void setBaseHeight(double baseHeight);
    void setCuttingToolRadius(double radius);
    void setInbetweenDistance(double distance);
    void setWorkingArea(glm::dvec3 blockSize, glm::dmat4 worldMatrix);
    void setObjectContours(const std::vector<glm::dvec3>& objectContour);

    void bake();
    std::vector<PathMovement> buildPaths();

protected:
    void createUncutZigZags();
    void subdivideZigZags();

private:
    ZigZagRejectionMode _rejectionMode;
    std::vector<glm::dvec3> _objectContour;
    double _toolRadius;
    double _inbetweenDistance;
    double _baseHeight;
    double _safeHeight;
    glm::dvec3 _blockSize;
    glm::dmat4 _blockWorldMatrix;
    glm::dmat4 _blockWorldMatrixInv;
    std::vector<std::vector<glm::dvec3>> _paths;

    std::vector<std::vector<std::tuple<glm::dvec2, glm::dvec2>>> _segments;
};

}
