set(
    NESpp_HEADERS
    BitMappedRegister.h
    CPU.h
    NES.h
)

set(
    NESpp_SOURCES
    main.cpp
    CPU.cpp
    NES.cpp
)

add_executable(
    NESpp
    ${NESpp_HEADERS}
    ${NESpp_SOURCES}
)