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
    void buildLinkMap();
    void followZigZag();
    void followLocalZigZag(int line, int segment);
    void followContour(
        std::vector<glm::dvec3> &collection,
        int from,
        int to,
        int increase = 1
    );

    glm::dvec3 findSafeEntryPoint(const glm::dvec3& pos) const;

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

    using SegmentTuple = std::tuple<glm::dvec2, glm::dvec2, int, int, int>;

    std::vector<std::vector<SegmentTuple>> _segments;
    std::vector<std::tuple<int, int, int>> _intersectionDetails;
    std::vector<std::tuple<int, int, int, int>> _linkMap;
};

}
