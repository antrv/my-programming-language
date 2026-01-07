#include <gtest/gtest.h>
#include "parser/details/CharParser.h"
#include "parser/details/OptionalParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;

TEST(OptionalParserTests, Success)
{
    constexpr auto parser = OptionalParser {CharParser {'a'}};

    constexpr std::string_view input {"a"sv};
    ParserContext<char> ctx {input};
    std::optional<char> value;
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, 'a');
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_TRUE(ctx.input().empty());
}

TEST(OptionalParserTests, EmptyInput)
{
    constexpr auto parser = OptionalParser {CharParser {'a'}};

    constexpr std::string_view input;
    ParserContext<char> ctx {input};
    std::optional<char> value;
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, std::nullopt);
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_EQ(std::string_view {ctx.input()}, input);
}

TEST(OptionalParserTests, InvalidInput)
{
    constexpr auto parser = OptionalParser {CharParser {'a'}};

    constexpr std::string_view input {"b"sv};
    ParserContext<char> ctx {input};
    std::optional<char> value;
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, std::nullopt);
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_EQ(std::string_view {ctx.input()}, input);
}
