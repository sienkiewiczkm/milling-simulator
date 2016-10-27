#include "MillingBlock.hpp"
#include "MillingErrors.hpp"
#include "DebugShapes.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <algorithm>
#include <iterator>
#include <sstream>

#include "BoundariesMillingTechnique.hpp"
#include "SamplingMillingTechnique.hpp"

using namespace fw;
using namespace glm;
using namespace std;

namespace ms
{

MillingBlock::MillingBlock():
    _blockSize(150.0f, 50.0f, 150.0f),
    _blockResolution(1024, 1024),
    _safeZoneHeight(30.0f)
{
    create();
}

MillingBlock::MillingBlock(
    glm::vec3 blockSize,
    glm::ivec2 blockResolution,
    float safeZoneHeight
):
    _blockSize(blockSize),
    _blockResolution(blockResolution),
    _safeZoneHeight(safeZoneHeight)
{
    create();
}

MillingBlock::~MillingBlock()
{
}

void MillingBlock::setModelMatrix(const glm::mat4 &modelMatrix)
{
    _modelMatrix = modelMatrix;
}

void MillingBlock::setViewMatrix(const glm::mat4 &viewMatrix)
{
    _viewMatrix = viewMatrix;
}

void MillingBlock::setProjectionMatrix(const glm::mat4 &projMatrix)
{
    _projMatrix = projMatrix;
}

void MillingBlock::setTexture(GLuint texture)
{
    _texture = texture;
}

void MillingBlock::setBlockSize(const glm::vec3 &blockSize)
{
    _blockSize = blockSize;
}

void MillingBlock::setBlockResolution(const glm::ivec2 &blockResolution)
{
    _blockResolution = blockResolution;
}

MillingError MillingBlock::moveTool(
    glm::dvec3 tipStartPoint,
    glm::dvec3 tipEndPoint, 
    const CuttingToolParams &cuttingToolParams,
    bool cuttingProhibited
)
{
    auto invScaling = scale(
        mat4(),
        vec3(1.0f/_blockSize.x, 1.0f/_blockSize.y, 1.0f/_blockSize.z)
    );

    auto tipTexMat = _textureMatrix * invScaling;

    auto selectedTechnique = _techniques.back();
    for (auto &tech : _techniques)
    {
        if (tech->isAvailable(tipStartPoint, tipEndPoint))
        {
            selectedTechnique = tech;
            break;
        }
    }

    auto errorState = selectedTechnique->moveTool(
        _rawHeightmap, 
        _blockResolution,
        tipTexMat,
        _safeZoneHeight,
        cuttingToolParams,
        tipStartPoint,
        tipEndPoint,
        cuttingProhibited
    );

    return errorState;
}

float MillingBlock::getSafeZoneHeight() const
{
    return _safeZoneHeight;
}

void MillingBlock::setSafeZoneHeight(float safeZoneHeight)
{
    _safeZoneHeight = safeZoneHeight;
}

void setSafeZoneHeight(float height);

glm::vec3 MillingBlock::getSafePosition() const
{
    return glm::vec3(0.0f, _blockSize.y + 10.0, 0.0f);
}

void MillingBlock::update()
{
    updateHeightmap();
}

void MillingBlock::render()
{
    _heightmapEffect->begin();
    _heightmapEffect->setModelMatrix(_modelMatrix);
    _heightmapEffect->setViewMatrix(_viewMatrix);
    _heightmapEffect->setProjectionMatrix(_projMatrix);
    _heightmapEffect->setTextureMatrix(_textureMatrix);
    _heightmapEffect->setAlbedoTexture(_texture);
    _heightmapEffect->setHeightmapTexture(_heightmapTexture);
    _heightmapEffect->setSize(vec2(_blockSize.x, _blockSize.z));
    _geometry->render();
    _heightmapEffect->end(); 

    glm::mat4 safeZoneTranformation = glm::translate(
        glm::mat4(),
        glm::vec3(0.0f, _safeZoneHeight, 0.0f)
    );

    _basicEffect->begin();
    _basicEffect->setModelMatrix(_modelMatrix * safeZoneTranformation);
    _basicEffect->setViewMatrix(_viewMatrix);
    _basicEffect->setProjectionMatrix(_projMatrix);
    _safeZoneLimitPlane->render();
    _basicEffect->end();
}

void MillingBlock::create()
{
    _heightmapEffect = make_shared<HeightmapVisualizationEffect>();
    _heightmapEffect->create();
    
    const float planeScale = 1.3f;
    _basicEffect = make_shared<fw::BasicEffect>();
    _basicEffect->create();

    _safeZoneLimitPlane = make_shared<Mesh<VertexNormalTexCoords>>(
        createPlane(planeScale * _blockSize.x, planeScale * _blockSize.z)
    );

    _heightmapTextureConverter = make_shared<HeightmapTextureConverter>();

    _geometry = make_shared<HeightmapGeometry>();
    _geometry->create(_blockResolution.x, _blockResolution.y, _blockSize);

    createHeightmap();

    _textureMatrix = translate(mat4(), vec3(0.5f, 0.0f, 0.5f));

    _techniques.push_back(make_shared<BoundariesMillingTechnique>());
    _techniques.push_back(make_shared<SamplingMillingTechnique>());
}

void MillingBlock::createHeightmap()
{
    _rawHeightmap.resize(_blockResolution.x * _blockResolution.y);
    fill(begin(_rawHeightmap), end(_rawHeightmap), _blockSize.y);

    _heightmapTexture = _heightmapTextureConverter->createTextureFromHeightmap(
        _rawHeightmap,
        _blockResolution.x,
        _blockResolution.y
    );
}

void MillingBlock::updateHeightmap()
{
    assert(_rawHeightmap.size() == (_blockResolution.x * _blockResolution.y));

    _heightmapTextureConverter->updateTextureFromHeightmap(
        _heightmapTexture,
        _rawHeightmap,
        _blockResolution.x,
        _blockResolution.y
    );
}

}
