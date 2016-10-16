#include "CuttingToolModel.hpp"
#include "DebugShapes.hpp"

using namespace std;

CuttingToolModel::CuttingToolModel() :
    _holderLength(0.0f),
    _holderRadius(0.0f),
    _shankLength(0.0f),
    _cuttingToolLength(0.0f),
    _cuttingToolRadius(0.0f),
    _cuttingToolBallRadius(0.0f)
{
}

CuttingToolModel::~CuttingToolModel()
{
    destroy();
}

void CuttingToolModel::create(
    float holderLength,
    float holderRadius,
    float shankLength,
    float cuttingToolLength,
    float cuttingToolRadius,
    float cuttingToolBallRadius
) {
    _holderLength = holderLength;
    _holderRadius = holderRadius;
    _shankLength = shankLength;
    _cuttingToolLength = cuttingToolLength;
    _cuttingToolRadius = cuttingToolRadius;
    _cuttingToolBallRadius = cuttingToolBallRadius;

    _holderMesh = make_shared<Mesh<VertexNormalTexCoords>>(
        createCylinder(_holderLength, _holderRadius, 32)
    );

    createBuffers();
}

void CuttingToolModel::destroy()
{
}

void CuttingToolModel::render()
{
    _holderMesh->render();
}

void CuttingToolModel::createBuffers()
{
}

void CuttingToolModel::destroyBuffers()
{
}
