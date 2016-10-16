#pragma once

#include "Mesh.hpp"
#include "Vertices.hpp"

#include <glm/glm.hpp>

class CuttingToolModel
{
public:
    CuttingToolModel();
    ~CuttingToolModel();

    void create(
        float holderLength,
        float holderRadius,
        float shankLength,
        float cuttingToolLength,
        float cuttingToolRadius,
        float cuttingToolBallRadius
    );

    void destroy();
    void render();

protected:
    void createBuffers();
    void destroyBuffers();

private:
    float _holderLength;
    float _holderRadius;
    float _shankLength;
    float _cuttingToolLength;
    float _cuttingToolRadius;
    float _cuttingToolBallRadius;

    std::shared_ptr<Mesh<VertexNormalTexCoords>> _holderMesh;
    std::shared_ptr<Mesh<VertexNormalTexCoords>> _shankMesh;
    std::shared_ptr<Mesh<VertexNormalTexCoords>> _cuttingToolMesh;
    std::shared_ptr<Mesh<VertexNormalTexCoords>> _cuttingToolBallMesh;
};
