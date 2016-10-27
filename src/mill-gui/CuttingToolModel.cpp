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

    createMesh();
}

void CuttingToolModel::destroy()
{
}

void CuttingToolModel::render(IEffect *effect)
{

    glm::mat4 ballMatrix = _modelMatrix * glm::translate(
        glm::mat4(),
        glm::vec3(0.0f, _cuttingToolBallRadius, 0.0f)
    );

    glm::mat4 ballScale = glm::scale(
        glm::mat4(),
        glm::vec3(
            _cuttingToolBallRadius,
            _cuttingToolBallRadius,
            _cuttingToolBallRadius
        )
    );
 
    glm::mat4 cuttingToolMatrix = ballMatrix * glm::translate(
        glm::mat4(),
        glm::vec3(0.0f, 0.5f * _cuttingToolLength, 0.0f)
    );

    glm::mat4 cuttingToolScale = glm::scale(
        glm::mat4(),
        glm::vec3(
            _cuttingToolRadius,
            _cuttingToolLength,
            _cuttingToolRadius
        )
    );

    glm::mat4 holderMatrix = cuttingToolMatrix * glm::translate(
        glm::mat4(),
        glm::vec3(0.0f, 0.5f * _cuttingToolLength + 0.5f * _holderLength, 0.0f)
    );

    glm::mat4 holderScale = glm::scale(
        glm::mat4(),
        glm::vec3(_holderRadius, _holderLength, _holderRadius)
    );

    effect->setModelMatrix(holderMatrix * holderScale);
    _holderMesh->render();

    effect->setModelMatrix(cuttingToolMatrix * cuttingToolScale);
    _cuttingToolMesh->render();

    effect->setModelMatrix(ballMatrix * ballScale);
    _cuttingToolBallMesh->render();
}

void CuttingToolModel::setModelMatrix(glm::mat4 modelMatrix)
{
    _modelMatrix = modelMatrix;
}


void CuttingToolModel::ensureCompability(const ms::CuttingToolParams &params)
{
    if (params.kind == ms::CuttingToolKind::Ball)
    {
        if (abs(params.radius - _cuttingToolRadius) > 0.0001f ||
            abs(params.radius - _cuttingToolBallRadius) > 0.0001f)
        {
            _cuttingToolRadius = _cuttingToolBallRadius = params.radius;
            return;
        }
    }

    if (params.kind == ms::CuttingToolKind::Flat)
    {
        if (abs(params.radius - _cuttingToolRadius) > 0.0001f ||
            abs(_cuttingToolBallRadius) > 0.0001f)
        {
            _cuttingToolBallRadius = 0.0f;
            _cuttingToolRadius = params.radius;
            return;
        }
    }
}

void CuttingToolModel::createMesh()
{
    _holderMesh = make_shared<Mesh<VertexNormalTexCoords>>(
        createCylinder(1.0f, 1.0f, 32)
    );

    _cuttingToolMesh = make_shared<Mesh<VertexNormalTexCoords>>(
        createCylinder(1.0f, 1.0f, 32)
    );

    _cuttingToolBallMesh = make_shared<Mesh<VertexNormalTexCoords>>(
        createSphere(1.0f, 32, 32)
    );
}
