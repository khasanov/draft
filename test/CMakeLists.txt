enable_testing()
include(GoogleTest)

add_executable(draft_test
    draft_test.cpp
    source_test.cpp
)

target_link_libraries(draft_test PRIVATE
    draft
    gtest_main
)
gtest_discover_tests(draft_test)
