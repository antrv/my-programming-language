#include <gtest/gtest.h>
#include "parser/details/CharParser.h"
#include "parser/details/OptionalParser.h"

using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(OptionalParserTests, OptionalParser)
{
    constexpr auto parser = OptionalParser {CharParser {'a'}};

    const auto result1 = parser.parse("a");
    ASSERT_TRUE(result1);
    EXPECT_EQ(result1.value(), 'a');

    const auto result2 = parser.parse("");
    ASSERT_FALSE(result2);
    const auto& msgs2 = result2.error();
    ASSERT_EQ(msgs2.size(), 1);
    EXPECT_EQ(msgs2[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(msgs2[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(msgs2[0].message, "Unexpected end of input, expected 'a'");
    EXPECT_EQ(msgs2[0].position, 0);
    EXPECT_EQ(msgs2[0].line, 1U);
    EXPECT_EQ(msgs2[0].column, 1U);

    const auto result3 = parser.parse("b");
    ASSERT_FALSE(result3);
    const auto& msgs3 = result3.error();
    ASSERT_EQ(msgs3.size(), 1);
    EXPECT_EQ(msgs3[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(msgs3[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(msgs3[0].message, "Unexpected input 'b', expected 'a'");
    EXPECT_EQ(msgs3[0].position, 0);
    EXPECT_EQ(msgs3[0].line, 1U);
    EXPECT_EQ(msgs3[0].column, 1U);
}
