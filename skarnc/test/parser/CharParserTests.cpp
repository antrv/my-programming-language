#include <gtest/gtest.h>
#include "parser/details/CharParser.h"

using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(CharParserTests, CharParser)
{
    constexpr CharParser parser {'a'};

    ParserContext<char> ctx1 {"a"};
    char value1 {};
    ASSERT_TRUE(parser.parse(ctx1, value1));
    EXPECT_EQ(value1, 'a');

    ParserContext<char> ctx2 {""};
    char value2 {};
    ASSERT_FALSE(parser.parse(ctx2, value2));
    const auto& msgs2 = ctx2.messages();
    ASSERT_EQ(msgs2.size(), 1);
    EXPECT_EQ(msgs2[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(msgs2[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(msgs2[0].message, "Unexpected end of input, expected 'a'");
    EXPECT_EQ(msgs2[0].position, 0);
    EXPECT_EQ(msgs2[0].line, 1U);
    EXPECT_EQ(msgs2[0].column, 1U);

    ParserContext<char> ctx3 {"b"};
    char value3 {};
    ASSERT_FALSE(parser.parse(ctx3, value3));
    const auto& msgs3 = ctx3.messages();
    ASSERT_EQ(msgs3.size(), 1);
    EXPECT_EQ(msgs3[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(msgs3[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(msgs3[0].message, "Unexpected input 'b', expected 'a'");
    EXPECT_EQ(msgs3[0].position, 0);
    EXPECT_EQ(msgs3[0].line, 1U);
    EXPECT_EQ(msgs3[0].column, 1U);
}
