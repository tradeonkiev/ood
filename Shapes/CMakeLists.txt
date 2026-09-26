cmake_minimum_required(VERSION 4.3)
project(Shapes LANGUAGES CXX)

add_library(shapes_lib
        lib/Canvas/SvgCanvas.cpp
        lib/Commands/CommandProcessor.cpp
        lib/Picture/Picture.cpp
        lib/Shape/Shape.cpp
        lib/Strategies/Ellipse/EllipseStrategy.cpp
        lib/Strategies/Triangle/TriangleStrategy.cpp
        lib/Strategies/Rectangle/RectangleStrategy.cpp
        lib/Strategies/Line/LineStrategy.cpp
        lib/Strategies/Text/TextStrategy.cpp)
target_include_directories(shapes_lib PUBLIC lib)
target_compile_features(shapes_lib PUBLIC cxx_std_23)
set_target_properties(shapes_lib PROPERTIES CXX_EXTENSIONS OFF)

add_executable(lw2_shapes main.cpp)
target_link_libraries(lw2_shapes PRIVATE shapes_lib)

include(CTest)
if (BUILD_TESTING)
    if (NOT TARGET GTest::gmock_main)
        include(FetchContent)
        FetchContent_Declare(
                googletest
                URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif ()

    add_executable(lw2_shapes_tests tests/ShapesTests.cpp)
    target_link_libraries(lw2_shapes_tests PRIVATE shapes_lib GTest::gmock_main)

    include(GoogleTest)
    gtest_discover_tests(lw2_shapes_tests)
endif ()
