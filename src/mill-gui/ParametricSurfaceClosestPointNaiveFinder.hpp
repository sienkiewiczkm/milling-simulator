#pragma once

#include "IParametricSurfaceUV.hpp"
#include <glm/glm.hpp>

namespace fw
{

class ParametricSurfaceClosestPointNaiveFinder
{
public:
    ParametricSurfaceClosestPointNaiveFinder();
    virtual ~ParametricSurfaceClosestPointNaiveFinder();

    void setReferencePoint(glm::dvec3 referencePoint);
    void setSamplingResolution(glm::ivec2 samplingResolution);

    glm::dvec2 find(const IParametricSurfaceUV& surface);

private:
    glm::dvec3 _referencePoint;
    glm::ivec2 _samplingResolution;
};

}
