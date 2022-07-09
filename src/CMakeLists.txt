cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Werror
    -Wall
    -Wextra
)

add_library(draft STATIC)
target_sources(draft PRIVATE
    arena.cpp
    arena.h
    ast.cpp
    ast.h
    ast_printer.cpp
    ast_printer.h
    builtin.cpp
    builtin.h
    driver.cpp
    driver.h
    environment.cpp
    environment.h
    interpreter.cpp
    interpreter.h
    lexer.cpp
    lexer.h
    obj_callable.h
    obj_class.cpp
    obj_class.h
    obj_function.cpp
    obj_function.h
    obj_instance.cpp
    obj_instance.h
    object.cpp
    object.h
    parser.cpp
    parser.h
    resolver.cpp
    resolver.h
    source.cpp
    source.h
    token.cpp
    token.def
    token.h
    unicode/basic_latin.def
)
target_include_directories(draft PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(draft-bin
    main.cpp
)

target_link_libraries(draft-bin PRIVATE
    draft
)

# rename drfat-bin executable to draft
set_target_properties(draft-bin PROPERTIES OUTPUT_NAME draft)
