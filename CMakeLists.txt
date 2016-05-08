cmake_minimum_required(VERSION 3.5)
project(PascalCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES example/syntax.cpp)
add_executable(PascalCompiler ${SOURCE_FILES})