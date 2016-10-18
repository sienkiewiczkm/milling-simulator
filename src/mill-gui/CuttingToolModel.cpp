#include "CuttingToolModel.hpp"
#include "DebugShapes.hpp"

#include <glm/gtc/matrix_transform.hpp>

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

    _cuttingToolMesh = make_shared<Mesh<VertexNormalTexCoords>>(
        createCylinder(_cuttingToolLength, _cuttingToolRadius, 32)
    );

    _cuttingToolBallMesh = make_shared<Mesh<VertexNormalTexCoords>>(
        createSphere(_cuttingToolBallRadius, 32, 32)
    );

    createBuffers();
}

void CuttingToolModel::destroy()
{
}

void CuttingToolModel::render(IEffect *effect)
{
    glm::mat4 ballMatrix = _modelMatrix * glm::translate(
        glm::mat4(),
        glm::vec3(
            0.0f,
            _cuttingToolBallRadius,
            0.0f
        )
    );
 
    glm::mat4 cuttingToolMatrix = ballMatrix * glm::translate(
        glm::mat4(),
        glm::vec3(
            0.0f,
            0.5f * _cuttingToolLength,
            0.0f
        )
    );

    glm::mat4 holderMatrix = cuttingToolMatrix * glm::translate(
        glm::mat4(),
        glm::vec3(
            0.0f,
            0.5f * _cuttingToolLength + 0.5f * _holderLength,
            0.0f
        )
    );

    effect->setModelMatrix(holderMatrix);
    _holderMesh->render();

    effect->setModelMatrix(cuttingToolMatrix);
    _cuttingToolMesh->render();

    effect->setModelMatrix(ballMatrix);
    _cuttingToolBallMesh->render();
}

void CuttingToolModel::setModelMatrix(glm::mat4 modelMatrix)
{
    _modelMatrix = modelMatrix;
}

void CuttingToolModel::createBuffers()
{
}

void CuttingToolModel::destroyBuffers()
{
}
