cmake_minimum_required (VERSION 2.6)
project (memobjects)

set(CMAKE_CXX_FLAGS "--std=c++17 -Wall -pedantic-errors -fPIC -Winit-self -Wold-style-cast -Woverloaded-virtual -Wuninitialized -Wmissing-declarations -Wunused-result -O3 -Wdouble-promotion -fconcepts -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g -O0")

add_executable(${PROJECT_NAME} main.cpp memobjects.h)
