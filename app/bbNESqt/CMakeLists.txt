set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 COMPONENTS Core Quick REQUIRED)

set(bbNESqt_SOURCES
    main.cpp
    main.qml
    qml.qrc)

add_executable(bbNESqt ${bbNESqt_SOURCES})
target_link_libraries(bbNESqt PRIVATE Qt6::Core Qt6::Quick NESpp)
