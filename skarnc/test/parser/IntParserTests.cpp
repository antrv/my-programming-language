#include <gtest/gtest.h>
#include "parser/details/IntParser.h"

using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(IntParserTests, IntParser)
{
    constexpr IntParser<int> parser;

    ParserContext<char> ctx1 {"a"};
    int value1 {};
    ASSERT_TRUE(parser.parse(ctx1, value1));
    EXPECT_EQ(value1, -123);

    ParserContext<char> ctx2 {""};
    int value2 {};
    ASSERT_FALSE(parser.parse(ctx2, value2));
    const auto& msgs2 = ctx2.messages();
    ASSERT_EQ(msgs2.size(), 1);
    EXPECT_EQ(msgs2[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(msgs2[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(msgs2[0].message, "Unexpected end of input, expected an integer");
    EXPECT_EQ(msgs2[0].position, 0);
    EXPECT_EQ(msgs2[0].line, 1U);
    EXPECT_EQ(msgs2[0].column, 1U);

    ParserContext<char> ctx3 {"b"};
    int value3 {};
    ASSERT_FALSE(parser.parse(ctx3, value3));
    const auto& msgs3 = ctx3.messages();
    ASSERT_EQ(msgs3.size(), 1);
    EXPECT_EQ(msgs3[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(msgs3[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(msgs3[0].message, "Unexpected input, expected an integer");
    EXPECT_EQ(msgs3[0].position, 0U);
    EXPECT_EQ(msgs3[0].line, 1U);
    EXPECT_EQ(msgs3[0].column, 1U);

    ParserContext<char> ctx4 {"3000000000"};
    int value4 {};
    ASSERT_FALSE(parser.parse(ctx4, value4));
    const auto& msgs4 = ctx4.messages();
    ASSERT_EQ(msgs4.size(), 1);
    EXPECT_EQ(msgs4[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(msgs4[0].code, ParserMsgCode::C0003);
    EXPECT_EQ(msgs4[0].message, "Unexpected input, expected an integer");
    EXPECT_EQ(msgs4[0].position, 0U);
    EXPECT_EQ(msgs4[0].line, 1U);
    EXPECT_EQ(msgs4[0].column, 1U);
}
