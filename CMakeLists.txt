cmake_minimum_required(VERSION 3.21)
project(lab_2_10)

set(CMAKE_CXX_STANDARD 17)

include_directories("inc/")

add_executable(lab_2_10 main.cpp inc/mathutils/matrix.hpp inc/BMP.hpp src/BMP.cpp)
