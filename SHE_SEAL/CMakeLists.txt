cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
project(SHE-Client)
find_package(SEAL 4.1 REQUIRED)
include_directories(src)

add_executable(SHE-Client src/main_interface.cpp)

if(TARGET SEAL::seal)
    target_link_libraries(SHE-Client SEAL::seal)
elseif(TARGET SEAL::seal_shared)
    target_link_libraries(SHE-Client SEAL::seal_shared)
endif()
