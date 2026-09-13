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

add_executable(shapes main.cpp)
target_link_libraries(shapes PRIVATE shapes_lib)

if (MINGW)
    target_link_options(shapes PRIVATE -static-libgcc -static-libstdc++)
endif ()
