#include "MillingBlock.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <algorithm>
#include <iterator>
#include <sstream>

using namespace fw;
using namespace glm;
using namespace std;

namespace ms
{

MillingBlock::MillingBlock():
    _blockSize(150.0f, 50.0f, 150.0f),
    _blockResolution(1024, 1024)
{
    create();
}

MillingBlock::MillingBlock(
    glm::vec3 blockSize,
    glm::ivec2 blockResolution
):
    _blockSize(blockSize),
    _blockResolution(blockResolution)
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

void MillingBlock::moveTool(
    glm::dvec3 tipStartPoint,
    glm::dvec3 tipEndPoint, 
    const CuttingToolParams &cuttingToolParams
)
{
    auto invScaling = scale(
        mat4(),
        vec3(1.0f/_blockSize.x, 1.0f/_blockSize.y, 1.0f/_blockSize.z)
    );

    auto tipTexMat = _textureMatrix * invScaling;

    _technique->moveTool(
        _rawHeightmap, 
        _blockResolution,
        tipTexMat,
        cuttingToolParams,
        tipStartPoint,
        tipEndPoint
    );
}

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
}

void MillingBlock::create()
{
    _heightmapEffect = make_shared<HeightmapVisualizationEffect>();
    _heightmapEffect->create();

    _heightmapTextureConverter = make_shared<HeightmapTextureConverter>();

    _geometry = make_shared<HeightmapGeometry>();
    _geometry->create(_blockResolution.x, _blockResolution.y, _blockSize);

    createHeightmap();

    _textureMatrix = translate(mat4(), vec3(0.5f, 0.0f, 0.5f));
    _technique = make_shared<BoundariesMillingTechnique>();
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
