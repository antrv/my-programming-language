#include <gtest/gtest.h>
#include "parser/details/ConstantParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;

TEST(ConstantParserTests, ValueParser)
{
    constexpr ConstantParser parser {12};

    constexpr std::string_view input {"input"sv};
    ParserContext<char> ctx {input};
    int value {};
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, 12);
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_EQ(std::string_view {ctx.input()}, input);
}
