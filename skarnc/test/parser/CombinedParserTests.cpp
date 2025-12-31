#include <gtest/gtest.h>
#include "parser/details/CharParser.h"
#include "parser/details/CombinedParser.h"
#include "parser/details/LiteralParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(CombinedParserTests, CombinedParser)
{
    constexpr auto parser = makeCombinedParser(
        LiteralParser {"template"}, CharParser {' '}, LiteralParser {"class"});

    ParserContext<char> ctx1 {"template class 1234"};
    std::tuple<std::string_view, char, std::string_view> value1 {};
    ASSERT_TRUE(parser.parse(ctx1, value1));
    EXPECT_EQ(value1, std::make_tuple("template"sv, ' ', "class"sv));

    ParserContext<char> ctx2 {""};
    std::tuple<std::string_view, char, std::string_view> value2 {};
    ASSERT_FALSE(parser.parse(ctx2, value2));
    const auto& msgs2 = ctx2.messages();
    ASSERT_EQ(msgs2.size(), 1);
    EXPECT_EQ(msgs2[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(msgs2[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(msgs2[0].message, "Unexpected end of input, expected 'template'");
    EXPECT_EQ(msgs2[0].position, 0);
    EXPECT_EQ(msgs2[0].line, 1U);
    EXPECT_EQ(msgs2[0].column, 1U);

    ParserContext<char> ctx3 {""};
    std::tuple<std::string_view, char, std::string_view> value3 {};
    ASSERT_FALSE(parser.parse(ctx3, value3));
    const auto& msgs3 = ctx3.messages();
    ASSERT_EQ(msgs3.size(), 1);
    EXPECT_EQ(msgs3[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(msgs3[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(msgs3[0].message, "Unexpected input 'xxx bbb', expected 'template'");
    EXPECT_EQ(msgs3[0].position, 0);
    EXPECT_EQ(msgs3[0].line, 1U);
    EXPECT_EQ(msgs3[0].column, 1U);
}
