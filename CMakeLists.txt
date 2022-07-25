cmake_minimum_required(VERSION 3.18)

project(draft)

include(dependencies.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(test)
