cmake_minimum_required(VERSION 4.0)
project(DisplayLibraryrary VERSION 0.1.0 LANGUAGES CXX)

# Build the display library
add_subdirectory(src)
add_subdirectory(tests)

# Console demo application that showcases the library
add_executable(twoRowDisplayApp 
    main.cpp 
    src/ConsoleInputListener.cpp
)

target_link_libraries(twoRowDisplayApp PRIVATE DisplayLibrary)

enable_testing()