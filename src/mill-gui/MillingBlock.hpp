#pragma once

#include "HeightmapGeometry.hpp"
#include "HeightmapTextureConverter.hpp"
#include "HeightmapVisualizationEffect.hpp"
#include "OpenGLHeaders.hpp"
#include "ISegmentMillingTechnique.hpp"
#include "CuttingToolParams.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace ms
{

class MillingBlock
{
public:
    MillingBlock();
    ~MillingBlock();

    void setModelMatrix(const glm::mat4 &modelMatrix);
    void setViewMatrix(const glm::mat4 &viewMatrix);
    void setProjectionMatrix(const glm::mat4 &projMatrix);

    void setTexture(GLuint texture);

    void setBlockSize(const glm::vec3 &blockSize);
    void setBlockResolution(const glm::ivec2 &blockResolution);

    void moveTool(
        glm::dvec3 tipStartPoint,
        glm::dvec3 tipEndPoint, 
        const CuttingToolParams &cuttingToolParams
    );

    void update();
    void render();

protected:
    void createHeightmap();
    void updateHeightmap();

private:
    glm::mat4 _modelMatrix;
    glm::mat4 _viewMatrix;
    glm::mat4 _projMatrix;
    glm::mat4 _textureMatrix;

    glm::vec3 _blockSize;
    glm::ivec2 _blockResolution;

    GLuint _texture;

    std::vector<float> _rawHeightmap;

    std::shared_ptr<HeightmapVisualizationEffect> _heightmapEffect;
    std::shared_ptr<HeightmapGeometry> _geometry;
    std::shared_ptr<fw::HeightmapTextureConverter> _heightmapTextureConverter;
    std::shared_ptr<ISegmentMillingTechnique> _technique;

    GLuint _heightmapTexture;
};

}