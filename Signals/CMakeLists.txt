cmake_minimum_required(VERSION 4.3)
project(Signals LANGUAGES CXX)

# Boost.Signals2 header-only: сначала ищем установленный Boost, иначе скачиваем только signals2 и его зависимости
find_package(Boost 1.80 CONFIG QUIET)
if (Boost_FOUND)
    set(SIGNALS2_TARGET Boost::headers)
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
    set(SIGNALS2_TARGET Boost::signals2)
endif ()

add_library(signals_lib
        lib/Canvas/SvgCanvas.cpp
        lib/Commands/CommandProcessor.cpp
        lib/Picture/Picture.cpp
        lib/Shape/Shape.cpp
        lib/Strategies/Ellipse/EllipseStrategy.cpp
        lib/Strategies/Triangle/TriangleStrategy.cpp
        lib/Strategies/Rectangle/RectangleStrategy.cpp
        lib/Strategies/Line/LineStrategy.cpp
        lib/Strategies/Text/TextStrategy.cpp)
target_include_directories(signals_lib PUBLIC lib)
target_link_libraries(signals_lib PUBLIC ${SIGNALS2_TARGET})
target_compile_features(signals_lib PUBLIC cxx_std_23)
set_target_properties(signals_lib PROPERTIES CXX_EXTENSIONS OFF)

add_executable(lw2_signals main.cpp)
target_link_libraries(lw2_signals PRIVATE signals_lib)

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

    add_executable(lw2_signals_tests
            tests/PictureSignalsTests.cpp
            tests/Signals2SemanticsTests.cpp)
    target_link_libraries(lw2_signals_tests PRIVATE signals_lib GTest::gmock_main)

    include(GoogleTest)
    gtest_discover_tests(lw2_signals_tests)
endif ()
