#include "DecalViewer.hpp"
#include <array>
#include "glm/gtc/type_ptr.hpp"
#include "imgui.h"
#include "DebugShapes.hpp"
#include "TextureUtils.hpp"
#include "Config.hpp"

namespace ms
{

DecalViewer::DecalViewer():
    _showDecal{false},
    _showManipulator{false},
    _manipulatorOperation{ImGuizmo::TRANSLATE},
    _decalPlane{createPlane(1.0f, 1.0f)},
    _texture{0}
{
    _texturedPhong = std::make_shared<TexturedPhongEffect>();
    _texturedPhong->create();
}

DecalViewer::~DecalViewer()
{
}

void DecalViewer::update(glm::mat4 view, glm::mat4 projection)
{
    _view = view;
    _proj = projection;

    if (!ImGui::Begin("Decal Viewer"))
    {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Show decal", &_showDecal);
    ImGui::Checkbox("Show manipulator", &_showManipulator);

    if (_showManipulator)
    {
        if (ImGui::RadioButton(
            "Translate",
            _manipulatorOperation == ImGuizmo::TRANSLATE
        ))
        {
            _manipulatorOperation = ImGuizmo::TRANSLATE;
        }

        ImGui::SameLine();

        if (ImGui::RadioButton(
            "Rotate",
            _manipulatorOperation == ImGuizmo::ROTATE
        ))
        {
            _manipulatorOperation = ImGuizmo::ROTATE;
        }

        ImGui::SameLine();

        if (ImGui::RadioButton(
            "Scale",
            _manipulatorOperation == ImGuizmo::SCALE
        ))
        {
            _manipulatorOperation = ImGuizmo::SCALE;
        }

        ImGuizmo::Manipulate(
            glm::value_ptr(_view),
            glm::value_ptr(_proj),
            _manipulatorOperation,
            ImGuizmo::LOCAL,
            glm::value_ptr(_model),
            nullptr,
            nullptr
        );
    }

    std::array<char, 256> decalNameText{};
    std::copy(
        std::begin(_textureName),
        std::end(_textureName),
        std::begin(decalNameText)
    );

    ImGui::Text("Place decal texture in ./assets/textures/");
    ImGui::InputText("Decal image", decalNameText.data(), 256);
    _textureName = decalNameText.data();

    if (ImGui::Button("Load decal image"))
    {
        ImGui::Text(_textureName.c_str());
        _texture = loadTextureFromFile(
            std::string(RESOURCES_PATH) + "textures/" + _textureName
        );
    }

    ImGui::End();
}

void DecalViewer::render()
{
    _texturedPhong->begin();
    _texturedPhong->setViewMatrix(_view);
    _texturedPhong->setProjectionMatrix(_proj);
    _texturedPhong->setTexture(_texture);
    _texturedPhong->setModelMatrix(_model);
    _decalPlane.render();
    _texturedPhong->end();
}

}
