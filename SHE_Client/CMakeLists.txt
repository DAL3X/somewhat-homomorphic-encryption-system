cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
project(SHE-Client)
include_directories(src)
add_executable(SHE-Client src/main_interface.cpp)

