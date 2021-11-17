include(FetchContent)

FetchContent_Declare(
    DocTest
    GIT_REPOSITORY https://github.com/onqtam/doctest
    GIT_TAG 4d8716f1efc1d14aa736ef52ee727bd4204f4c40 # release 2.4.6
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG d141cdbeb0fb422a3fb7173b285fd38e0d1772dc # release 8.0.1
)

add_subdirectory(doctest)
add_subdirectory(fmt)
