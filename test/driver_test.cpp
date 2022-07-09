#include <gtest/gtest.h>

#include <sstream>

#include <driver.h>

using namespace draft;

TEST(DraftTest, Smoke)
{
    constexpr auto greeting = "Hello, world!";
    std::stringstream ss;
    io::writeLine(greeting, ss);

    ASSERT_EQ("Hello, world!\n", ss.str());
}
