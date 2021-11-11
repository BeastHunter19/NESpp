set(
    NESpp_HEADERS
    Debugger.h
    Emulator.h
)

set(
    NESpp_SOURCES
    BitMappedRegister.h
    CPU.h
    CPU.cpp
    NES.h
    NES.cpp
    EmulatorCore.h
    Debugger.cpp
    Emulator.cpp
)

list(TRANSFORM NESpp_HEADERS PREPEND include/NESpp/)
list(TRANSFORM NESpp_SOURCES PREPEND src/)

add_library(NESpp STATIC ${NESpp_SOURCES} ${NESpp_HEADERS})

target_include_directories(NESpp INTERFACE include)
target_include_directories(NESpp PRIVATE include/NESpp PUBLIC src)
