cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Werror
    -Wall
    -Wextra
)

add_library(libraft STATIC)
target_sources(libraft PRIVATE
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
target_include_directories(libraft PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(raft
    main.cpp
)

target_link_libraries(raft PRIVATE
    libraft
)

