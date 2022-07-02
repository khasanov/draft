#include <gtest/gtest.h>

#include <sstream>

#include <draft.h>

using namespace draft;

TEST(DraftTest, Smoke)
{
    constexpr auto greeting = "Hello, world!";
    std::stringstream ss;
    Draft::out(greeting, ss);

    ASSERT_EQ("Hello, world!\n", ss.str());
}
