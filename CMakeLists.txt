cmake_minimum_required(VERSION 3.0)
project(milling-simulator)

find_library(OpenGL REQUIRED)

# glfw: do not build docs, tests and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/glfw)

include_directories(dependencies/glm)
include_directories(dependencies/stb)

set(RESOURCES_PATH ${PROJECT_SOURCE_DIR}/resources/)

configure_file(
    ${PROJECT_SOURCE_DIR}/src/Config.hpp.in 
    ${PROJECT_BINARY_DIR}/Config.hpp
)

include_directories(
    ${PROJECT_BINARY_DIR}
    dependencies/imgui
    dependencies/generated/glad/include
)

add_executable(${PROJECT_NAME} 
    src/mill-gui/DebugShapes.cpp
    src/mill-gui/HeightmapGeometry.cpp
    src/mill-gui/ImGuiBinding.cpp
    src/mill-gui/Main.cpp
    src/mill-gui/MillerApplication.cpp
    src/mill-gui/OpenGLApplication.cpp
    src/mill-gui/OrbitingCamera.cpp
    src/mill-gui/Shaders.cpp
    src/mill-gui/StbImplementation.c
    src/mill-gui/TextureUtils.cpp
    src/mill-gui/Vertices.cpp
    dependencies/imgui/imgui.cpp
    dependencies/imgui/imgui_draw.cpp
    dependencies/generated/glad/src/glad.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} 
    glfw
    ${OPENGL_gl_LIBRARY}
)


target_compile_features(${PROJECT_NAME} PRIVATE
    cxx_auto_type
    cxx_nullptr
    cxx_range_for
)

