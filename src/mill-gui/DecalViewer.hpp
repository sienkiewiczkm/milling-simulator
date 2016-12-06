#pragma once

#include <string>
#include "glm/glm.hpp"
#include "ImGuizmo.h"
#include "Mesh.hpp"
#include "TexturedPhongEffect.hpp"
#include "Vertices.hpp"

namespace ms
{

class DecalViewer
{
public:
    DecalViewer();
    ~DecalViewer();

    void update(glm::mat4 view, glm::mat4 projection);
    void render();

private:
    glm::mat4 _view, _proj, _model;
    bool _showDecal;
    bool _showManipulator;
    ImGuizmo::OPERATION _manipulatorOperation;

    std::shared_ptr<TexturedPhongEffect> _texturedPhong;

    GLuint _texture;
    std::string _textureName;
    glm::mat4 _decalTransformation;
    Mesh<VertexNormalTexCoords> _decalPlane;
};

}
