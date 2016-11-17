#include "DesignModeController.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <ImGuizmo.h>

#include "CommonBsplineSurfaces.hpp"
#include "Config.hpp"
#include "DebugShapes.hpp"
#include "EquidistantParametricSurface.hpp"
#include "ParametricSurfaceIntersectionFinder.hpp"

namespace ms
{

DesignModeController::DesignModeController():
    _activeOperation{ImGuizmo::TRANSLATE},
    _blockSize{150.0f, 50.0f, 150.0f},
    _baseHeight{21.5f},
    _displayLimits{false},
    _gizmosEnabled{false},
    _selectedGizmo{0},
    _probeEnabled{false},
    _intersectionsReady{false}
{
}

DesignModeController::~DesignModeController()
{
}

void DesignModeController::setWindow(GLFWwindow *window)
{
    _window = window;
}

void DesignModeController::onCreate()
{
    _effect.create();
    _texture = loadTextureFromFile(RESOURCE("textures/opengameart-metal.jpg"));

    _basicEffect = make_shared<fw::BasicEffect>();
    _basicEffect->create();

    CadioModelLoader loader;
    _loadedObjects = loader.loadModel(RESOURCE("models/sienkiewiczk.model"));

    _loadedModelMatrix = glm::translate(
        _loadedModelMatrix,
        glm::vec3{0, 20.05f, 0}
    );

    _loadedModelMatrix = glm::rotate(
        _loadedModelMatrix,
        glm::radians(90.0f),
        glm::vec3{1.0f, 0, 0}
    );

    _loadedModelMatrix = glm::scale(
        _loadedModelMatrix,
        glm::vec3(20.05f, 23.05f, 22.95f)
    );

    _baseBspline = std::make_shared<fw::BsplineNonVanishingReparametrization>(
        fw::createBsplinePlane(
            {-90.0, _baseHeight, -90.0},
            {90.0, _baseHeight, -90.0},
            {-90.0, _baseHeight, 90.0},
            {90.0, _baseHeight, 90.0},
            glm::inverse(_loadedModelMatrix)
        )
    );

    _modelIntersections =
        std::make_shared<SienkiewiczkModelIntersectionsHandler>(
            _loadedObjects[0],
            _loadedObjects[1],
            _loadedObjects[2],
            _baseBspline
        );

    glm::dvec4 origin{0,0,0,1};
    glm::dvec4 radius{6,6,6,1};
    auto scaledToolRadius = glm::inverse(_loadedModelMatrix) * radius
        - glm::inverse(_loadedModelMatrix) * origin;
    _modelIntersections->setScaledToolRadius(scaledToolRadius.y);

    fw::ParametricSurfaceMeshBuilder parametricBuilder;
    parametricBuilder.setSamplingResolution(glm::ivec2(64, 64));

    _baseBsplineMesh = parametricBuilder.build(_baseBspline);

    _roughPathGenerator = std::make_shared<RoughMillingPathGenerator>();
    _roughPathGenerator->setCuttingToolRadius(8.0f);
    _roughPathGenerator->setBaseHeight(_baseHeight);
    _roughPathGenerator->setWorkingArea(_blockSize, _baseBoxModelMatrix);
    _roughPathGenerator->setWorkingAreaResolution({300, 300});//0.5mm

    _curveMillingPathGenerator = std::make_shared<CurvePathGenerator>();
    _curveMillingPathGenerator->setBaseHeight(_baseHeight);

    _preciseMillingPathGenerator =
        std::make_shared<PreciseMillingPathGenerator>();
    _preciseMillingPathGenerator->setBaseHeight(_baseHeight);

    for (const auto &object: _loadedObjects)
    {
        _loadedObjectMeshes.push_back(
            parametricBuilder.build(object)
        );

        _roughPathGenerator->addParametricSurface(
            object,
            _loadedModelMatrix
        );
    }

    _flatteningPathGenerator = std::make_shared<BaseFlatteningPathGenerator>();
    _flatteningPathGenerator->setBaseHeight(_baseHeight);
    _flatteningPathGenerator->setCuttingToolRadius(6.0f);
    _flatteningPathGenerator->setWorkingArea(_blockSize, _baseBoxModelMatrix);


    createMeshes();
}

void DesignModeController::onActivate()
{
}

void DesignModeController::onDeactivate()
{
}

void DesignModeController::onDestroy()
{
}

void DesignModeController::onUpdate(float deltaTime)
{
    if (ImGui::Begin("Design settings"))
    {
        updateMainWindow();
    }
    ImGui::End();
}

void DesignModeController::onRender(const OrbitingCamera &orbitingCamera)
{
    // todo: should be glfw agnostic
    int display_w, display_h;
    glfwGetFramebufferSize(_window, &display_w, &display_h);

    glm::mat4 model, view, projection;
    float aspectRatio = (float)display_w/display_h;

    model = glm::scale(glm::mat4(), glm::vec3(1.0f, 1.0f, 1.0f));
    view = orbitingCamera.getViewMatrix();
    projection = glm::perspective(45.0f, aspectRatio, 5.0f, 700.0f);

    if (_gizmosEnabled)
    {
        ImGuizmo::OPERATION activeOperation;
        switch (_selectedGizmo)
        {
        case 0:
            activeOperation = ImGuizmo::TRANSLATE;
            break;
        case 1:
            activeOperation = ImGuizmo::ROTATE;
            break;
        case 2:
            activeOperation = ImGuizmo::SCALE;
            break;
        }

        ImGuizmo::Manipulate(
            glm::value_ptr(view),
            glm::value_ptr(projection),
            activeOperation,
            ImGuizmo::LOCAL,
            glm::value_ptr(_loadedModelMatrix),
            nullptr,
            nullptr
        );
    }

    if (_probeEnabled)
    {
        ImGuizmo::Manipulate(
            glm::value_ptr(view),
            glm::value_ptr(projection),
            ImGuizmo::TRANSLATE,
            ImGuizmo::LOCAL,
            glm::value_ptr(_probeMatrix),
            nullptr,
            nullptr
        );
    }

    _effect.begin();
    _effect.setViewMatrix(view);
    _effect.setProjectionMatrix(projection);
    _effect.setTexture(_texture);

    _effect.setModelMatrix(_baseBoxModelMatrix);
    _baseBox->render();

    _effect.setModelMatrix(_loadedModelMatrix);

    for (const auto &mesh: _loadedObjectMeshes)
    {
        mesh->render();
    }

    _effect.end();

    _basicEffect->begin();
    _basicEffect->setViewMatrix(view);
    _basicEffect->setProjectionMatrix(projection);
    _basicEffect->setModelMatrix(_loadedModelMatrix);

    glDisable(GL_DEPTH_TEST);
    _modelIntersections->render();
    glEnable(GL_DEPTH_TEST);

    if (_displayLimits)
    {
        _basicEffect->setModelMatrix(_blockBoxLimitsModelMatrix);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        _blockBoxLimits->render();
        glDisable(GL_BLEND);
    }

    _basicEffect->end();
}

void DesignModeController::updateMainMenuBar()
{
}

void DesignModeController::onKey(int key, int scancode, int action, int mods)
{
    switch (key)
    {
    case GLFW_KEY_Q:
        break;
    case GLFW_KEY_W:
        _activeOperation = ImGuizmo::TRANSLATE;
        break;
    case GLFW_KEY_E:
        _activeOperation = ImGuizmo::ROTATE;
        break;
    case GLFW_KEY_R:
        _activeOperation = ImGuizmo::SCALE;
        break;
    }
}

void DesignModeController::createMeshes()
{
    _basicEffect = make_shared<fw::BasicEffect>();
    _basicEffect->create();

    _baseBox = createBox(glm::vec3{_blockSize.x, _baseHeight, _blockSize.z});
    _baseBoxModelMatrix = glm::translate(
        glm::mat4(),
        glm::vec3{0, _baseHeight/2, 0}
    );

    glm::vec3 deltaSize(0.001f, 0.001f, 0.001f);
    _blockBoxLimits = createBox(_blockSize - deltaSize);
    _blockBoxLimitsModelMatrix = glm::translate(
        glm::mat4(),
        glm::vec3{0, _blockSize.y/2, 0}
    );
}

void DesignModeController::updateMainWindow()
{
    if (ImGui::CollapsingHeader("Visuals"))
    {
        ImGui::Checkbox("Display limits", &_displayLimits);
    }

    if (ImGui::CollapsingHeader("Measurements"))
    {
        ImGui::Checkbox("Enable position probe", &_probeEnabled);
        if (_probeEnabled)
        {
            float matrixTranslation[3], matrixRotation[3], matrixScale[3];

            ImGuizmo::DecomposeMatrixToComponents(
                glm::value_ptr(glm::inverse(_loadedModelMatrix) * _probeMatrix),
                matrixTranslation,
                matrixRotation,
                matrixScale
            );

            ImGui::InputFloat3("Translation", matrixTranslation, 3);
        }
    }

    if (ImGui::CollapsingHeader("Object placement"))
    {
        ImGui::Checkbox("Enable gizmos", &_gizmosEnabled);

        const char *labels[] = { "Translate", "Rotate", "Scale" };
        ImGui::Combo("Active gizmo", &_selectedGizmo, labels, 3);

        float matrixTranslation[3], matrixRotation[3], matrixScale[3];

        ImGuizmo::DecomposeMatrixToComponents(
            glm::value_ptr(_loadedModelMatrix),
            matrixTranslation,
            matrixRotation,
            matrixScale
        );

        ImGui::InputFloat3("Translation", matrixTranslation, 3);
        ImGui::InputFloat3("Rotation", matrixRotation, 3);
        ImGui::InputFloat3("Scale", matrixScale, 3);

        ImGuizmo::RecomposeMatrixFromComponents(
            matrixTranslation,
            matrixRotation,
            matrixScale,
            glm::value_ptr(_loadedModelMatrix)
        );
    }

    if (ImGui::CollapsingHeader("Baking"))
    {
        if (ImGui::Button("Rough milling process"))
        {
            _roughPathGenerator->bake();
            auto program = _roughPathGenerator->buildPaths();

            MillPathFormatWriter writer;
            writer.writeToFile(RESOURCE("paths/last_roughing.f16"), program);

            executor->setProgram("Local program", program);
            CuttingToolParams defaultParameters;
            defaultParameters.kind = CuttingToolKind::Ball;
            defaultParameters.radius = 8.0f;
            executor->getController()->setCuttingToolParams(defaultParameters);
        }

        if (ImGui::Button("Preprocess intersections"))
        {
            _modelIntersections->findIntersections();
            _flatteningPathGenerator->setObjectContours(
                _modelIntersections->getObjectContour(_loadedModelMatrix)
            );
            _intersectionsReady = true;
        }

        if (_intersectionsReady
            && ImGui::Button("Flatten base around the object"))
        {
            _flatteningPathGenerator->bake();
            auto program = _flatteningPathGenerator->buildPaths();

            MillPathFormatWriter writer;
            writer.writeToFile(RESOURCE("paths/last_flattening.f12"), program);

            executor->setProgram("Local program (flattening)", program);
            CuttingToolParams defaultParameters;
            defaultParameters.kind = CuttingToolKind::Flat;
            defaultParameters.radius = 6.0f;
            executor->getController()->setCuttingToolParams(defaultParameters);
        }

        // body handle drill <- order
        if (_intersectionsReady && ImGui::Button("Mill DRILL precisely"))
        {
            _preciseMillingPathGenerator->setParametricSurface(
                _loadedObjects[2],
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->setParametricSurfaceBoundaries(
                _modelIntersections->getDrillParametricContours()[0]
            );

            _preciseMillingPathGenerator->clearCheckSurfaces();
            _preciseMillingPathGenerator->addCheckSurface(
                _loadedObjects[0],
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->setCuttingToolRadius(4.0);
            _preciseMillingPathGenerator->setNumScanLines(64);
            _preciseMillingPathGenerator->setNumLineMaximumResolution(64);
            _preciseMillingPathGenerator->bake();
            auto program = _preciseMillingPathGenerator->buildPaths();

            MillPathFormatWriter writer;
            writer.writeToFile(RESOURCE("paths/last_drill.k8"), program);

            executor->setProgram("Local program (drill)", program);
            CuttingToolParams defaultParameters;
            defaultParameters.kind = CuttingToolKind::Ball;
            defaultParameters.radius = 4.0f;
            executor->getController()->setCuttingToolParams(defaultParameters);
        }

        if (_intersectionsReady && ImGui::Button("Mill HANDLE precisely"))
        {
            _preciseMillingPathGenerator->setParametricSurface(
                _loadedObjects[1],
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->setParametricSurfaceBoundaries(
                _modelIntersections->getHandleParametricContours()[0]
            );

            _preciseMillingPathGenerator->clearCheckSurfaces();
            _preciseMillingPathGenerator->addCheckSurface(
                _loadedObjects[0],
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->setCuttingToolRadius(4.0);
            _preciseMillingPathGenerator->setNumScanLines(64);
            _preciseMillingPathGenerator->setNumLineMaximumResolution(64);
            _preciseMillingPathGenerator->bake();
            auto program = _preciseMillingPathGenerator->buildPaths();

            MillPathFormatWriter writer;
            writer.writeToFile(RESOURCE("paths/last_handle.k8"), program);

            executor->setProgram("Local program (handle)", program);
            CuttingToolParams defaultParameters;
            defaultParameters.kind = CuttingToolKind::Ball;
            defaultParameters.radius = 4.0f;
            executor->getController()->setCuttingToolParams(defaultParameters);
        }

        if (_intersectionsReady && ImGui::Button("Mill BODY precisely"))
        {
            _preciseMillingPathGenerator->setParametricSurface(
                _loadedObjects[0],
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->setParametricSurfaceBoundaries(
                _modelIntersections->getBodyParametricContours()[0]
            );

            _preciseMillingPathGenerator->clearCheckSurfaces();
            _preciseMillingPathGenerator->addCheckSurface(
                _loadedObjects[1],
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->addCheckSurface(
                _loadedObjects[2],
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->setCuttingToolRadius(4.0);
            _preciseMillingPathGenerator->setNumScanLines(128);
            _preciseMillingPathGenerator->setNumLineMaximumResolution(64);
            _preciseMillingPathGenerator->bake();
            auto program = _preciseMillingPathGenerator->buildPaths();

            MillPathFormatWriter writer;
            writer.writeToFile(RESOURCE("paths/last_body.k8"), program);

            executor->setProgram("Local program (body)", program);
            CuttingToolParams defaultParameters;
            defaultParameters.kind = CuttingToolKind::Ball;
            defaultParameters.radius = 4.0f;
            executor->getController()->setCuttingToolParams(defaultParameters);
        }

        if (_intersectionsReady && ImGui::Button("Mill HANDLE HOLE precisely"))
        {
            _preciseMillingPathGenerator->setParametricSurface(
                _baseBspline,
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->setParametricSurfaceBoundaries(
                _modelIntersections->getBaseParametricContours()[0]
            );

            _preciseMillingPathGenerator->clearCheckSurfaces();

            _preciseMillingPathGenerator->addCheckSurface(
                _loadedObjects[0],
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->addCheckSurface(
                _loadedObjects[1],
                _loadedModelMatrix
            );

            _preciseMillingPathGenerator->setCuttingToolRadius(0.5);
            _preciseMillingPathGenerator->setNumScanLines(512);
            _preciseMillingPathGenerator->setNumLineMaximumResolution(64);
            _preciseMillingPathGenerator->bake();
            auto program = _preciseMillingPathGenerator->buildPaths();

            MillPathFormatWriter writer;
            writer.writeToFile(RESOURCE("paths/last_handle_hole.k1"), program);

            executor->setProgram("Local program (handle hole)", program);
            CuttingToolParams defaultParameters;
            defaultParameters.kind = CuttingToolKind::Ball;
            defaultParameters.radius = 0.5f;
            executor->getController()->setCuttingToolParams(defaultParameters);
        }

        if (_intersectionsReady && ImGui::Button("Refine connections"))
        {
            _curveMillingPathGenerator->setTransform(_loadedModelMatrix);
            _curveMillingPathGenerator->setRefinementCurves(
                _modelIntersections->getRefinementCurves()
            );

            _curveMillingPathGenerator->bake();
            auto program = _curveMillingPathGenerator->buildPaths();

            MillPathFormatWriter writer;
            writer.writeToFile(RESOURCE("paths/last_refinement.k8"), program);

            executor->setProgram("Local program (refinement)", program);
            CuttingToolParams defaultParameters;
            defaultParameters.kind = CuttingToolKind::Ball;
            defaultParameters.radius = 4.0f;
            executor->getController()->setCuttingToolParams(defaultParameters);
        }
    }

    if (ImGui::CollapsingHeader("Parametric previews"))
    {
        renderParametricPreviewCanvas(
            "Drill",
            _modelIntersections->getDrillParametricContours()
        );

        ImGui::Separator();

        renderParametricPreviewCanvas(
            "Handle",
            _modelIntersections->getHandleParametricContours()
        );

        ImGui::Separator();

        renderParametricPreviewCanvas(
            "Body",
            _modelIntersections->getBodyParametricContours()
        );

        ImGui::Separator();

        renderParametricPreviewCanvas(
            "Base",
            _modelIntersections->getBaseParametricContours()
        );
    }
}

void DesignModeController::renderParametricPreviewCanvas(
    std::string name,
    const std::vector<std::vector<glm::dvec2>>& contours
)
{
    std::string title = "Parametric preview of \"" + name + "\"";
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImGui::Text(title.c_str());

    ImVec4 bg = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    ImVec4 fg = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 fglight = ImVec4(0.5f, 0.0f, 0.0f, 1.0f);
    const ImU32 bg32 = ImColor(bg);
    const ImU32 fg32 = ImColor(fg);
    const ImU32 fglight32 = ImColor(fglight);

    float offset = 8;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImVec2 size = ImVec2(512, 512);
    ImVec2 p0{
        p.x + offset,
        p.y + offset
    };

    draw_list->AddRectFilled(
        p0,
        ImVec2(p0.x + size.x, p0.y + size.y),
        bg32
    );

    for (auto& contour: contours)
    {
        for (auto i = 0; i+1 < contour.size(); ++i)
        {
            if (contour[i].x < 0 || contour[i].x > 1
                || contour[i].y < 0 || contour[i].y > 1
                || contour[i+1].x < 0 || contour[i+1].x > 1
                || contour[i+1].y < 0 || contour[i+1].y > 1)
            {
                continue;
            }

            ImVec2 start{
                static_cast<float>(contour[i].x * size.x),
                static_cast<float>(contour[i].y * size.y)
            };

            ImVec2 end{
                static_cast<float>(contour[i+1].x * size.x),
                static_cast<float>(contour[i+1].y * size.y)
            };

            /*
            draw_list->AddCircle(
                ImVec2(p0.x + start.x, p0.y + start.y),
                4.0f,
                fglight32
            );

            draw_list->AddCircle(
                ImVec2(p0.x + end.x, p0.y + end.y),
                4.0f,
                fglight32
            );
            */

            draw_list->AddLine(
                ImVec2(start.x + p0.x, start.y + p0.y),
                ImVec2(end.x + p0.x, end.y + p0.y),
                fg32
            );
        }
    }

    ImGui::Dummy(ImVec2(offset + size.x, offset + size.y));
}

}
