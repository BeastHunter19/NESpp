set(
    NESpp_TEST_SOURCES
    test_main.cpp
)

add_executable(TestMain ${NESpp_TEST_SOURCES})
target_link_libraries(TestMain PRIVATE doctest::doctest)
add_test(NAME MainTest COMMAND TestMain)