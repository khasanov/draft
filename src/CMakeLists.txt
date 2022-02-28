cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Werror
    -Wall
    -Wextra
)

add_executable(raft
    arena.cpp
    arena.h
    ast.cpp
    ast.h
    ast_printer.cpp
    ast_printer.h
    environment.cpp
    environment.h
    interpreter.cpp
    interpreter.h
    main.cpp
    object.cpp
    object.h
    parser.cpp
    parser.h
    raft.cpp
    raft.h
    scanner.cpp
    scanner.h
    token.cpp
    token.def
    token.h
)
