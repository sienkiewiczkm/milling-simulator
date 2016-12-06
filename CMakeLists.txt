cmake_minimum_required(VERSION 3.0)
project(milling-simulator)

set(PROJECT_NAME_LIB ${PROJECT_NAME}-lib)
set(DEPENDENCIES_DIR dependencies/)

# configure boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.60.0 COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

set(RESOURCES_PATH ${PROJECT_SOURCE_DIR}/resources/)

set(FRAMEWORK_RESOURCES_DIR ${RESOURCES_PATH} CACHE PATH "")
add_subdirectory(${DEPENDENCIES_DIR}/framework)

configure_file(
    ${PROJECT_SOURCE_DIR}/src/Config.hpp.in
    ${PROJECT_BINARY_DIR}/Config.hpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/mill-gui/
    ${PROJECT_BINARY_DIR}
    ${FRAMEWORK_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME_LIB}
    src/mill-gui/BaseFlatteningPathGenerator.cpp
    src/mill-gui/BoundariesMillingTechnique.cpp
    src/mill-gui/CadioModelLoader.cpp
    src/mill-gui/ContourMerger.cpp
    src/mill-gui/CurvePathGenerator.cpp
    src/mill-gui/CuttingToolController.cpp
    src/mill-gui/CuttingToolGUI.cpp
    src/mill-gui/CuttingToolModel.cpp
    src/mill-gui/CuttingToolParams.cpp
    src/mill-gui/DecalViewer.cpp
    src/mill-gui/DesignModeController.cpp
    src/mill-gui/ImGuiToolboxWindow.cpp
    src/mill-gui/MillPathFormatReader.cpp
    src/mill-gui/MillerApplication.cpp
    src/mill-gui/MillingBlock.cpp
    src/mill-gui/MillingBlockCreationWindow.cpp
    src/mill-gui/MillingProgramExecutor.cpp
    src/mill-gui/MillingProgramExecutorGUI.cpp
    src/mill-gui/PreciseMillingPathGenerator.cpp
    src/mill-gui/ProgramManagerGUI.cpp
    src/mill-gui/RoughMillingPathGenerator.cpp
    src/mill-gui/SamplingMillingTechnique.cpp
    src/mill-gui/SienkiewiczkModelIntersectionsHandler.cpp
    src/mill-gui/SimulationModeController.cpp
    src/mill-gui/ZigZagPathGenerator.cpp
)

add_executable(${PROJECT_NAME}
    src/mill-gui/Main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    ${PROJECT_NAME_LIB}
    ${Boost_LIBRARIES}
    ${FRAMEWORK_LIBRARIES}
)

set(PROJECT_COMPILE_FEATURES
    ${PROJECT_COMPILE_FEATURES}
    cxx_auto_type
    cxx_nullptr
    cxx_range_for
)

target_compile_features(${PROJECT_NAME} PRIVATE ${PROJECT_COMPILE_FEATURES})
target_compile_features(${PROJECT_NAME_LIB} PRIVATE ${PROJECT_COMPILE_FEATURES})
