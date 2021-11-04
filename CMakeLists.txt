cmake_minimum_required(VERSION 3.17)

project(
    NESpp 
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tests)
