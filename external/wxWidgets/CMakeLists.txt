message(STATUS "Fetching wxWidgets...")
set(wxBUILD_SHARED OFF CACHE INTERNAL "Build STATIC library")
FetchContent_MakeAvailable(wxWidgets)