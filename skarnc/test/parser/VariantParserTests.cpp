#include <gtest/gtest.h>
#include "parser/details/IntParser.h"
#include "parser/details/LiteralParser.h"
#include "parser/details/VariantParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(VariantParserTests, FirstValue)
{
    constexpr auto parser = makeVariantParser(IntParser<unsigned> {}, LiteralParser {"value"sv});

    constexpr std::string_view input {"123456789"sv};
    ParserContext<char> ctx {input};
    std::variant<unsigned, std::string_view> value;
    ASSERT_TRUE(parser.parse(ctx, value));
    ASSERT_EQ(value.index(), 0);
    EXPECT_EQ(std::get<0>(value), 123456789U);
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_TRUE(ctx.input().empty());
}

TEST(VariantParserTests, SecondValue)
{
    constexpr auto parser = makeVariantParser(IntParser<unsigned> {}, LiteralParser {"value"sv});

    constexpr std::string_view input {"value"sv};
    ParserContext<char> ctx {input};
    std::variant<unsigned, std::string_view> value;
    ASSERT_TRUE(parser.parse(ctx, value));
    ASSERT_EQ(value.index(), 1);
    EXPECT_EQ(std::get<1>(value), "value"sv);
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_TRUE(ctx.input().empty());
}

TEST(VariantParserTests, EmptyInput)
{
    constexpr auto parser = makeVariantParser(IntParser<unsigned> {}, LiteralParser {"value"sv});

    constexpr std::string_view input;
    ParserContext<char> ctx {input};
    std::variant<unsigned, std::string_view> value;
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(messages[0].expected, "'value'"sv);
    EXPECT_EQ(messages[0].position, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}

TEST(VariantParserTests, InvalidInput)
{
    constexpr auto parser = makeVariantParser(IntParser<unsigned> {}, LiteralParser {"value"sv});

    constexpr std::string_view input {"test"sv};
    ParserContext<char> ctx {input};
    std::variant<unsigned, std::string_view> value;
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(messages[0].expected, "'value'"sv);
    EXPECT_EQ(messages[0].position, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}
