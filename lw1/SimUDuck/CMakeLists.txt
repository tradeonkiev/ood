cmake_minimum_required(VERSION 4.3)
project(SimUDuck LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(simuduck SimUDuck.cpp)

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

    add_executable(duck_dance_tests tests/DuckDanceTests.cpp)
    target_link_libraries(duck_dance_tests PRIVATE GTest::gmock_main)

    include(GoogleTest)
    gtest_discover_tests(duck_dance_tests)
endif ()
