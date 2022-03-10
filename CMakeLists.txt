cmake_minimum_required(VERSION 3.18)

project(raft)

include(dependencies.cmake)

add_subdirectory(src)
add_subdirectory(test)
