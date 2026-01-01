#include <gtest/gtest.h>
#include "parser/details/IntParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(IntParserTests, Success)
{
    constexpr IntParser<int> parser;

    constexpr std::string_view input {"-123"sv};
    ParserContext<char> ctx {input};
    int value {};
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, -123);
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_TRUE(ctx.input().empty());
}

TEST(IntParserTests, EmptyInput)
{
    constexpr IntParser<int> parser;

    constexpr std::string_view input;
    ParserContext<char> ctx {input};
    int value {};
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(messages[0].message, "Unexpected end of input, expected an integer");
    EXPECT_EQ(messages[0].position, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}

TEST(IntParserTests, InvalidInput)
{
    constexpr IntParser<int> parser;

    constexpr std::string_view input {"b"sv};
    ParserContext<char> ctx {input};
    int value {};
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(messages[0].message, "Unexpected input, expected an integer");
    EXPECT_EQ(messages[0].position, 0U);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}

TEST(IntParserTests, Overflow)
{
    constexpr IntParser<int> parser;

    constexpr std::string_view input {"3000000000"sv};
    ParserContext<char> ctx {input};
    int value {};
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0003);
    EXPECT_EQ(messages[0].message, "An integer is out of range");
    EXPECT_EQ(messages[0].position, 0U);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}
