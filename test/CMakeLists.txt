enable_testing()
include(GoogleTest)

add_executable(draft-test
    draft_test.cpp
    source_test.cpp
)

target_link_libraries(draft-test PRIVATE
    draft
    gtest_main
)
gtest_discover_tests(draft-test)
