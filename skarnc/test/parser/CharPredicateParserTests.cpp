#include <gtest/gtest.h>
#include "parser/details/CharPredicateParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(CharPredicateParserTests, Success)
{
    constexpr CharPredicateParser parser {
        [](const char chr) static constexpr noexcept {
            return chr >= 'a' && chr <= 'z';
        }, "letter a..z"};

    constexpr std::string_view input {"d"sv};
    ParserContext<char> ctx {input};
    char value {};
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, 'd');
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_TRUE(ctx.input().empty());
}

TEST(CharPredicateParserTests, EmptyInput)
{
    constexpr CharPredicateParser parser {
        [](const char chr) static constexpr noexcept {
            return chr >= 'a' && chr <= 'z';
        }, "letter a..z"};

    constexpr std::string_view input;
    ParserContext<char> ctx {input};
    char value {};
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(messages[0].message, "Unexpected end of input, expected letter a..z"sv);
    EXPECT_EQ(messages[0].position, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}

TEST(CharPredicateParserTests, InvalidInput)
{
    constexpr CharPredicateParser parser {
        [](const char chr) static constexpr noexcept {
            return chr >= 'a' && chr <= 'z';
        }, "letter a..z"};

    constexpr std::string_view input {"9"sv};
    ParserContext<char> ctx {input};
    char value {};
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(messages[0].message, "Unexpected input '9', expected letter a..z"sv);
    EXPECT_EQ(messages[0].position, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}
