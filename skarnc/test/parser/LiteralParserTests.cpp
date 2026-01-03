#include <gtest/gtest.h>
#include "parser/details/LiteralParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(LiteralParserTests, Success)
{
    constexpr LiteralParser parser {"template"sv};

    constexpr std::string_view input {"template"sv};
    ParserContext<char> ctx {input};
    std::string_view value;
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, "template"sv);
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_TRUE(ctx.input().empty());
}

TEST(LiteralParserTests, EmptyInput)
{
    constexpr LiteralParser parser {"template"sv};

    constexpr std::string_view input;
    ParserContext<char> ctx {input};
    std::string_view value;
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(messages[0].expected, "'template'"sv);
    EXPECT_EQ(messages[0].offset, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}

TEST(LiteralParserTests, InvalidInput)
{
    constexpr LiteralParser parser {"template"sv};

    constexpr std::string_view input {"xxx bbb"sv};
    ParserContext<char> ctx {input};
    std::string_view value;
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(messages[0].expected, "'template'"sv);
    EXPECT_EQ(messages[0].offset, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}
