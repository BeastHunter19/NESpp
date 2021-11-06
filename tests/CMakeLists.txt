set(
    NESpp_TEST_SOURCES
    test_main.cpp
    test_CPU.cpp
)

add_executable(TestMain ${NESpp_TEST_SOURCES})
target_include_directories(TestMain PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(TestMain PRIVATE doctest::doctest NESpp)
add_test(NAME MainTest COMMAND TestMain)