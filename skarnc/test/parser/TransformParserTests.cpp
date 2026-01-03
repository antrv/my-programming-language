#include <gtest/gtest.h>
#include "parser/details/CharParser.h"
#include "parser/details/TransformParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(TransformParserTests, Success)
{
    constexpr TransformParser parser {CharParser {'a'},
        [](int& value, const char& c) constexpr static noexcept {
            value = static_cast<unsigned char>(c);
        }};

    constexpr std::string_view input {"a"sv};
    ParserContext<char> ctx {input};
    int value {};
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, 'a');
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_TRUE(ctx.input().empty());
}

TEST(TransformParserTests, EmptyInput)
{
    constexpr TransformParser parser {CharParser {'a'},
        [](int& value, const char& c) constexpr static noexcept {
            value = static_cast<unsigned char>(c);
        }};

    constexpr std::string_view input;
    ParserContext<char> ctx {input};
    int value {};
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(messages[0].expected, "'a'"sv);
    EXPECT_EQ(messages[0].offset, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}

TEST(TransformParserTests, InvalidInput)
{
    constexpr TransformParser parser {CharParser {'a'},
        [](int& value, const char& c) constexpr static noexcept {
            value = static_cast<unsigned char>(c);
        }};

    constexpr std::string_view input {"b"sv};
    ParserContext<char> ctx {input};
    int value {};
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(messages[0].expected, "'a'"sv);
    EXPECT_EQ(messages[0].offset, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}
