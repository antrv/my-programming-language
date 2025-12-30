#include <gtest/gtest.h>
#include "parser/details/ValueParser.h"

using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(ValueParserTests, ValueParser)
{
    constexpr ValueParser parser {12};

    ParserContext<char> ctx1 {"input"};
    int value1 {};
    ASSERT_TRUE(parser.parse(ctx1, value1));
    EXPECT_EQ(value1, 12);
}
