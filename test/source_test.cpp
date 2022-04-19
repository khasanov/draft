#include <gtest/gtest.h>

#include <source.h>

using namespace raft;

TEST(SourceTest, toStdString)
{
    std::u32string utf32{U"Ӏ"};  // U+04C0
    ASSERT_EQ(1, utf32.size());

    std::string utf8 = Source::toStdString(utf32);
    ASSERT_EQ(2, utf8.size());
}

TEST(SourceTest, fromStdString)
{
    std::string utf8{"Ӏ"};  // U+04C0
    ASSERT_EQ(2, utf8.size());

    std::u32string utf32 = Source::fromStdString(utf8);
    ASSERT_EQ(1, utf32.size());
}
