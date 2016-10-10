cmake_minimum_required(VERSION 3.0)
project(milling-simulator)

find_library(OpenGL REQUIRED)

# glfw: do not build docs, tests and examples
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/glfw)

set(ASSETS_PATH_PREFIX ${PROJECT_SOURCE_DIR}/assets/)
set(SHADER_PATH_PREFIX ${PROJECT_SOURCE_DIR}/src/shaders/)

# configure_file(
#  ${PROJECT_SOURCE_DIR}/src/config.hpp.in 
#  ${PROJECT_BINARY_DIR}/config.hpp
# )

include_directories(
    ${PROJECT_BINARY_DIR}
    dependencies/generated/glad/include
)

add_executable(${PROJECT_NAME} 
    src/mill-gui/main.cpp
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

