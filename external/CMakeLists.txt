include(FetchContent)

FetchContent_Declare(
    DocTest
    GIT_REPOSITORY https://github.com/onqtam/doctest
    GIT_TAG 4d8716f1efc1d14aa736ef52ee727bd4204f4c40 # release 2.4.6
)

add_subdirectory(doctest)