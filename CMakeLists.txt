cmake_minimum_required(VERSION 3.10.0)
project(twoRowDisplayLibrary VERSION 0.1.0 LANGUAGES C CXX)

add_executable(twoRowDisplayLibrary 
    main.cpp 
    ConsoleInputListener.cpp 
    ConsoleRenderer.cpp
)

include_directories(.)

