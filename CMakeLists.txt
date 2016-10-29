cmake_minimum_required(VERSION 3.0)
project(milling-simulator)

set(PROJECT_NAME_LIB ${PROJECT_NAME}-lib)
set(PROJECT_NAME_TEST ${PROJECT_NAME}-test)
set(DEPENDENCIES_DIR dependencies/)

find_library(OpenGL REQUIRED)

if (WIN32)
    set(GLFW_ADDITIONAL_DEFINITIONS
        "${GLFW_ADDITIONAL_DEFINITIONS} GLFW_EXPOSE_NATIVE_WIN32"
    )

    set(ADDITIONAL_DEFINITIONS
        "${ADDITIONAL_DEFINITIONS} GLFW_EXPOSE_NATIVE_WIN32"
    )
endif()

# glfw: do not build docs, tests and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${DEPENDENCIES_DIR}glfw)
target_compile_definitions(glfw PRIVATE ${GLFW_ADDITIONAL_DEFINITIONS})

# configure boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.60.0 COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(${DEPENDENCIES_DIR}googletest)
enable_testing()
include_directories(${gtest_SOURCE_DIR}/include)

include_directories(dependencies/glfw/include)
include_directories(dependencies/glm)
include_directories(dependencies/stb)

set(RESOURCES_PATH ${PROJECT_SOURCE_DIR}/resources/)

configure_file(
    ${PROJECT_SOURCE_DIR}/src/Config.hpp.in
    ${PROJECT_BINARY_DIR}/Config.hpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/mill-gui/
    ${PROJECT_BINARY_DIR}
    dependencies/imgui
    dependencies/generated/glad/include
)

add_library(${PROJECT_NAME_LIB}
    src/mill-gui/BasicEffect.cpp
    src/mill-gui/BoundariesMillingTechnique.cpp
    src/mill-gui/Common.cpp
    src/mill-gui/CuttingToolController.cpp
    src/mill-gui/CuttingToolGUI.cpp
    src/mill-gui/CuttingToolModel.cpp
    src/mill-gui/CuttingToolParams.cpp
    src/mill-gui/DebugShapes.cpp
    src/mill-gui/Effect.cpp
    src/mill-gui/HeightmapGeometry.cpp
    src/mill-gui/HeightmapTextureConverter.cpp
    src/mill-gui/HeightmapVisualizationEffect.cpp
    src/mill-gui/ImGuiBinding.cpp
    src/mill-gui/ImGuiToolboxWindow.cpp
    src/mill-gui/MillPathFormatReader.cpp
    src/mill-gui/MillerApplication.cpp
    src/mill-gui/MillingBlock.cpp
    src/mill-gui/MillingBlockCreationWindow.cpp
    src/mill-gui/MillingProgramExecutor.cpp
    src/mill-gui/MillingProgramExecutorGUI.cpp
    src/mill-gui/OpenGLApplication.cpp
    src/mill-gui/OrbitingCamera.cpp
    src/mill-gui/ProgramManagerGUI.cpp
    src/mill-gui/SamplingMillingTechnique.cpp
    src/mill-gui/Shaders.cpp
    src/mill-gui/StbImplementation.c
    src/mill-gui/TextureUtils.cpp
    src/mill-gui/TexturedPhongEffect.cpp
    src/mill-gui/Vertices.cpp
    dependencies/imgui/imgui.cpp
    dependencies/imgui/imgui_demo.cpp
    dependencies/imgui/imgui_draw.cpp
    dependencies/generated/glad/src/glad.c
)

add_executable(${PROJECT_NAME_TEST}
    test/CommonTest.cpp
)

add_executable(${PROJECT_NAME}
    src/mill-gui/Main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME_TEST} 
    ${PROJECT_NAME_LIB}
    gtest
    gtest_main
)

target_link_libraries(${PROJECT_NAME}
    ${PROJECT_NAME_LIB}
    glfw
    ${OPENGL_gl_LIBRARY}
    ${Boost_LIBRARIES}
)

target_compile_definitions(${PROJECT_NAME_LIB} PRIVATE
    ${ADDITIONAL_DEFINITIONS}
)

set(PROJECT_COMPILE_FEATURES
    cxx_auto_type
    cxx_nullptr
    cxx_range_for
)

target_compile_features(${PROJECT_NAME} PRIVATE ${PROJECT_COMPILE_FEATURES})
target_compile_features(${PROJECT_NAME_LIB} PRIVATE ${PROJECT_COMPILE_FEATURES})

add_test(NAME ${PROJECT_NAME_TEST} COMMAND ${PROJECT_NAME_TEST})
