#include <gtest/gtest.h>

#include "parser/Parser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;

TEST(ParserTests, Identifier) {
    constexpr auto identParser = Parse::char_([](const char c) constexpr static noexcept {
            return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
        }, "identifier"sv) >>
        *Parse::char_([](const char c) constexpr static noexcept {
            return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_';
        }, "identifier"sv);

    const auto result = identParser.parse("variableName123_xxx");
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), std::make_tuple('v', "ariableName123_xxx"sv));
}

TEST(ParserTests, IdentifierInvalidInput) {
    constexpr auto identParser = Parse::char_([](const char c) constexpr static noexcept {
            return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
        }, "identifier"sv) >>
        *Parse::char_([](const char c) constexpr static noexcept {
            return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_';
        }, "identifier"sv);

    const auto result = identParser.parse("123variable");
    ASSERT_FALSE(result);

    const auto& messages = result.error();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(messages[0].expected, "identifier"sv);
    EXPECT_EQ(messages[0].offset, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}
