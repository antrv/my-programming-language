#include <gtest/gtest.h>
#include "parser/details/IntParser.h"
#include "parser/details/ReferenceParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;

TEST(ReferenceParserTests, Success)
{
    constexpr ReferenceParser<int> parser;
    constexpr IntParser<int> intParser;
    parser.assign(intParser);

    constexpr std::string_view input {"-123"sv};
    ParserContext<char> ctx {input};
    int value {};
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(value, -123);
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_TRUE(ctx.input().empty());
}
