enable_testing()
include(GoogleTest)

add_executable(raft_test
    raft_test.cpp
)

target_link_libraries(raft_test PRIVATE
    libraft
    gtest_main
)
gtest_discover_tests(raft_test)
