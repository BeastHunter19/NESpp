set(
    EmulatorMain_SOURCES
    main.cpp
)

add_executable(EmulatorMain ${EmulatorMain_SOURCES})
target_link_libraries(EmulatorMain PRIVATE NESpp)

add_subdirectory(bbNESqt)

