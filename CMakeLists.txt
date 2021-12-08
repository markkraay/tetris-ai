cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
project(tetris VERSION 1.0)
set(CMAKE_BUILD_TYPE Debug)

option(BUILD_PYTHON "Build Python modules. (OFF builds executable)" OFF)

add_compile_definitions(tetris "RESOURCE_PATH=\"${CMAKE_SOURCE_DIR}/resources/\"")
if (BUILD_PYTHON) 
    add_compile_definitions(BUILD_PYTHON)
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/include/*.hpp)

find_package(SFML 2.5 COMPONENTS REQUIRED network audio graphics window system)

if (BUILD_PYTHON) 
    message("Building pybind11 modules for Python implementation.")
    find_package(pybind11 REQUIRED)
    pybind11_add_module(tetris_environment ${SRC_FILES})
    target_link_libraries(tetris_environment PRIVATE sfml-graphics sfml-window)
else()
    message("Building default executable.")
    add_executable(tetris ${SRC_FILES})
    target_link_libraries(tetris sfml-graphics sfml-window)
endif()

