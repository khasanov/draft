cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Werror
    -Wall
    -Wextra
)

add_executable(raft
    main.cpp
    raft.cpp
    raft.h
    scanner.cpp
    scanner.h
    token.cpp
    token.h
)
