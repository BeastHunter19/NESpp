cmake_minimum_required(VERSION 3.17...3.21)

project(
    NESpp 
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)
include(CTest)

add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(app)
add_subdirectory(tests)
