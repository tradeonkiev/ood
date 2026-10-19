cmake_minimum_required(VERSION 4.3)
project(Editor LANGUAGES CXX)

if (NOT TARGET signals_lib)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Signals ${CMAKE_CURRENT_BINARY_DIR}/Signals)
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
        Model/Commands/AddShape/AddShapeCommand.cpp
        Model/Commands/ChangeBounds/ChangeBoundsCommand.cpp
        Model/Commands/DeleteShape/DeleteShapeCommand.cpp
        Model/Commands/History.cpp
        Presenter/EditorPresenter.cpp
        Presenter/SelectionFrame.cpp)
target_include_directories(editor_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(editor_app PUBLIC signals_lib)
target_compile_features(editor_app PUBLIC cxx_std_23)
set_target_properties(editor_app PROPERTIES CXX_EXTENSIONS OFF)

add_library(editor_sfml
        View/SfmlCanvas.cpp
        View/SfmlEditorView.cpp)
target_link_libraries(editor_sfml PUBLIC editor_app SFML::Graphics)


add_executable(editor main.cpp)
target_link_libraries(editor PRIVATE editor_sfml)
