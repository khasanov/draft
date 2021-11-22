cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
    -Werror
    -Wall
    -Wextra
)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.72.0)

include_directories(
    ${Boost_INCLUDE_DIRS}
)

add_executable(raft
    main.cpp
)

target_link_libraries(raft
    LINK_PRIVATE ${Boost_LIBRARIES}
)

