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

list(TRANSFORM NESpp_HEADERS PREPEND include/NESpp/)
list(TRANSFORM NESpp_SOURCES PREPEND src/)

add_library(NESpp STATIC ${NESpp_SOURCES} ${NESpp_HEADERS})

target_include_directories(NESpp INTERFACE include)
target_include_directories(NESpp PRIVATE include/NESpp PUBLIC src)
