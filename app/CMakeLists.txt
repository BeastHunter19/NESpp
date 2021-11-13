set(
    EmulatorMain_SOURCES
    main.cpp
)

set(
    wxEmulator_SOURCES
    myApp.h
    myApp.cpp
    MyFrame.h
    MyFrame.cpp
)

add_executable(EmulatorMain ${EmulatorMain_SOURCES})
target_link_libraries(EmulatorMain PRIVATE NESpp)

add_executable(wxEmulator ${wxEmulator_SOURCES})
target_link_libraries(wxEmulator PRIVATE NESpp wx::core wx::base)