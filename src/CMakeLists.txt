# Display Library
# A generic display library with scrolling support

add_library(DisplayLibrary STATIC
    ConsoleRenderer.cpp
)

# Public headers that consumers of this library need
target_include_directories(DisplayLibrary PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set C++ standard for the library
target_compile_features(DisplayLibrary PUBLIC cxx_std_17)