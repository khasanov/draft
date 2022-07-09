#include <gtest/gtest.h>

#include <source.h>

using namespace draft;

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

TEST(Source, positionAt)
{
    std::string code{R"(print "Hello";
print "world";
)"};

    Source source{code};

    Source::Offset offset0{0};
    Source::Position pos1_1 = source.positionAt(offset0);
    ASSERT_EQ(source.charAt(offset0), U'p');
    ASSERT_EQ(1, pos1_1.line);
    ASSERT_EQ(1, pos1_1.column);

    Source::Offset offset7{7};
    ASSERT_TRUE(source.charAt(offset7) == U'H');
    Source::Position pos1_8 = source.positionAt(offset7);
    ASSERT_EQ(1, pos1_8.line);
    ASSERT_EQ(8, pos1_8.column);

    Source::Offset offset22{22};
    ASSERT_TRUE(source.charAt(offset22) == U'w');
    Source::Position pos2_8 = source.positionAt(offset22);
    ASSERT_EQ(2, pos2_8.line);
    ASSERT_EQ(8, pos2_8.column);
}
