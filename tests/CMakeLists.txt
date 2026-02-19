enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(DisplayLibraryTests
    DisplayItemTests.cpp
    DisplayControllerTests.cpp
    ScrollingTests.cpp
)

target_link_libraries(DisplayLibraryTests PRIVATE 
    DisplayLibrary
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(DisplayLibraryTests)