#include <gtest/gtest.h>
#include "parser/details/CharParser.h"
#include "parser/details/CombinedParser.h"
#include "parser/details/IgnoreParser.h"
#include "parser/details/LiteralParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(CombinedParserTests, Success)
{
    constexpr CombinedParser parser {LiteralParser {"template"sv}, CharParser {' '}, LiteralParser {"class"sv}};

    constexpr std::string_view input {"template class 1234"sv};
    ParserContext<char> ctx {input};
    std::tuple<std::string_view, char, std::string_view> value {};
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, std::make_tuple("template"sv, ' ', "class"sv));
    EXPECT_EQ(std::string_view {ctx.input()}, " 1234"sv);
}

TEST(CombinedParserTests, EmptyInput)
{
    constexpr CombinedParser parser {LiteralParser {"template"sv}, CharParser {' '}, LiteralParser {"class"sv}};

    constexpr std::string_view input;
    ParserContext<char> ctx {input};
    std::tuple<std::string_view, char, std::string_view> value {};
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

TEST(CombinedParserTests, PartiallyValidInput)
{
    constexpr CombinedParser parser {LiteralParser {"template"sv}, CharParser {' '}, LiteralParser {"class"sv}};

    constexpr std::string_view input {"template struct"sv};
    ParserContext<char> ctx {input};
    std::tuple<std::string_view, char, std::string_view> value {};
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::get<0>(value), "template"sv);
    EXPECT_EQ(std::get<1>(value), ' ');
    EXPECT_EQ(std::string_view {ctx.input()}, "struct"sv);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(messages[0].expected, "'class'"sv);
    EXPECT_EQ(messages[0].offset, 9);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 10U);
}

TEST(CombinedParserTests, InvalidInput)
{
    constexpr CombinedParser parser {LiteralParser {"template"sv}, CharParser {' '}, LiteralParser {"class"sv}};

    constexpr std::string_view input {"xxx bbb"sv};
    ParserContext<char> ctx {input};
    std::tuple<std::string_view, char, std::string_view> value {};
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

TEST(CombinedParserTests, IgnoreAllValues)
{
    constexpr CombinedParser parser {
        IgnoreParser {LiteralParser {"template"sv}},
        IgnoreParser {CharParser {' '}},
        IgnoreParser {LiteralParser {"class"sv}}};

    constexpr std::string_view input {"template class 1234"sv};
    ParserContext<char> ctx {input};
    ASSERT_TRUE(parser.parse(ctx));
    EXPECT_EQ(std::string_view {ctx.input()}, " 1234"sv);
}
