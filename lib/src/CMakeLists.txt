set(
    NESpp_HEADERS
    NES.h
)

set(
    NESpp_SOURCES
    BitMappedRegister.h
    CPU.h
    CPU.cpp
    NES.cpp
)

add_library(
    NESpp
    ${NESpp_SOURCES}
)

target_include_directories(NESpp INTERFACE ../include)
target_include_directories(NESpp PRIVATE ../include/NESpp)
