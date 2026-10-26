cmake_minimum_required(VERSION 4.3)
project(Editor LANGUAGES CXX)

find_package(Boost 1.80 CONFIG QUIET)
if (Boost_FOUND)
    set(EDITOR_BOOST_TARGET Boost::headers)
else ()
    include(FetchContent)
    set(BOOST_INCLUDE_LIBRARIES signals2)
    set(BOOST_ENABLE_CMAKE ON)
    FetchContent_Declare(
            Boost
            URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
            DOWNLOAD_EXTRACT_TIMESTAMP ON
            EXCLUDE_FROM_ALL)
    FetchContent_MakeAvailable(Boost)
    set(EDITOR_BOOST_TARGET Boost::signals2)
endif ()

find_package(SFML 3 COMPONENTS Graphics Window System CONFIG QUIET)
if (NOT SFML_FOUND)
    include(FetchContent)
    set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
            SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG 3.0.2
            GIT_SHALLOW ON
            EXCLUDE_FROM_ALL
            SYSTEM)
    FetchContent_MakeAvailable(SFML)
endif ()

add_library(editor_app
        IO/DocumentIO.cpp
        IO/LoadService.cpp
        IO/SaveService.cpp
        Model/Commands/AddShape/AddShapeCommand.cpp
        Model/Commands/ChangeBounds/ChangeBoundsCommand.cpp
        Model/Commands/DeleteShape/DeleteShapeCommand.cpp
        Model/Commands/Group/GroupCommand.cpp
        Model/Commands/History.cpp
        Model/Picture.cpp
        Model/ShapeFactory.cpp
        Model/Shapes/EllipseShape.cpp
        Model/Shapes/RectangleShape.cpp
        Model/Shapes/Shape.cpp
        Model/Shapes/TriangleShape.cpp
        ViewModel/EditorViewModel.cpp
        ViewModel/SelectionFrame.cpp)
target_include_directories(editor_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(editor_app PUBLIC ${EDITOR_BOOST_TARGET})
target_compile_features(editor_app PUBLIC cxx_std_23)
set_target_properties(editor_app PROPERTIES CXX_EXTENSIONS OFF)

add_library(editor_sfml
        View/FileDialog.cpp
        View/SfmlCanvas.cpp
        View/SfmlEditorView.cpp)
target_link_libraries(editor_sfml PUBLIC editor_app SFML::Graphics comdlg32)


add_executable(editor main.cpp)
target_link_libraries(editor PRIVATE editor_sfml)
