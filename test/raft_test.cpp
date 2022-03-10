#include <gtest/gtest.h>

#include <sstream>

#include <raft.h>

using namespace raft;

TEST(RaftTest, Smoke)
{
    constexpr auto greeting = "Hello, world!";
    std::stringstream ss;
    Raft::out(greeting, ss);

    ASSERT_EQ("Hello, world!\n", ss.str());
}
