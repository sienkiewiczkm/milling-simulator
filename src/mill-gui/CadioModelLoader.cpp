#include "CadioModelLoader.hpp"
#include "BsplineSurface.hpp"
#include "BsplineNonVanishingReparametrization.hpp"
#include "BsplineEquidistantKnotGenerator.hpp"

#include <fstream>
#include <iostream>

namespace ms
{

CadioModelLoader::CadioModelLoader()
{
}

CadioModelLoader::~CadioModelLoader()
{
}

std::vector<std::shared_ptr<fw::IParametricSurfaceUV>>
        CadioModelLoader::loadModel(
    const std::string path
)
{
    std::ifstream fileStream(path);
    if (!fileStream.is_open())
    {
        throw new std::string("Cannot open requested file.");
    }

    int numControlPoints;
    fileStream >> numControlPoints;

    std::vector<glm::vec3> controlPoints(numControlPoints);

    for (auto i = 0; i < numControlPoints; ++i)
    {
        fileStream >> controlPoints[i].x >> controlPoints[i].y
            >> controlPoints[i].z;
    }

    int numObjects;
    fileStream >> numObjects;

    std::vector<std::shared_ptr<fw::IParametricSurfaceUV>> objects(numObjects);

    std::shared_ptr<fw::IBsplineKnotGenerator> knotGenerator =
        std::static_pointer_cast<fw::IBsplineKnotGenerator>(
            std::make_shared<fw::BsplineEquidistantKnotGenerator>()
        );

    for (auto i = 0; i < numObjects; ++i)
    {
        std::string objectIdentifier, objectName;
        fileStream >> objectIdentifier;
        std::getline(fileStream, objectName);

        if (objectIdentifier != "BSPLINESURF")
        {
            std::string fileLine;
            while (std::getline(fileStream, fileLine) && fileLine != "END");
            continue;
        }

        int numControlPointsU, numControlPointsV;
        char surfaceType, foldDirection;

        fileStream >> numControlPointsU >> numControlPointsV;
        fileStream >> surfaceType >> foldDirection;

        std::vector<glm::dvec3> surfaceControlPoints;
        std::vector<int> surfaceControlPointsIndices;

        for (auto i = 0; i < numControlPointsU * numControlPointsV; ++i)
        {
            int vertexId{0};
            fileStream >> vertexId;
            surfaceControlPointsIndices.push_back(vertexId);
        }

        for (auto y = 0; y < numControlPointsV; ++y)
        {
            for (auto x = 0; x < numControlPointsU; ++x)
            {
                surfaceControlPoints.push_back(
                    controlPoints[surfaceControlPointsIndices[
                        numControlPointsU * y + x]
                    ]
                );
            }
        }

        fw::SurfaceFoldingMode foldingMode = fw::SurfaceFoldingMode::None;
        if (surfaceType == 'C')
        {
            foldingMode = foldDirection == 'V'
                ? fw::SurfaceFoldingMode::ContinuousU
                : fw::SurfaceFoldingMode::ContinuousV;
        }

        auto numControlPointsUFolded = numControlPointsU
            + (foldingMode == fw::SurfaceFoldingMode::ContinuousU ? 3 : 0);

        auto numControlPointsVFolded = numControlPointsV
            + (foldingMode == fw::SurfaceFoldingMode::ContinuousV ? 3 : 0);

        auto bsplineSurface = std::make_shared<fw::BsplineSurface>(
            3,
            glm::ivec2(numControlPointsU, numControlPointsV),
            surfaceControlPoints,
            knotGenerator->generate(numControlPointsUFolded, 3),
            knotGenerator->generate(numControlPointsVFolded, 3),
            foldingMode,
            3
        );

        std::string fileLine;
        while (std::getline(fileStream, fileLine)
            && fileLine.find("END") == std::string::npos)
        {
        }

        auto bsplineReparametrization =
            std::make_shared<fw::BsplineNonVanishingReparametrization>(
                bsplineSurface
            );

        objects[i] = std::static_pointer_cast<fw::IParametricSurfaceUV>(
            bsplineReparametrization
        );
    }

    return objects;
}

}
