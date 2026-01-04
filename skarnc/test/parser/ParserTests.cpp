#include <gtest/gtest.h>
#include "parser/Parser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;

namespace {
constexpr auto letterPredicate = [](const char c) static noexcept {
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
};

constexpr auto letterDigitPredicate = [](const char c) static noexcept {
    return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_';
};

constexpr auto identParser =
    Parse::char_(letterPredicate, "identifier"sv) >>
    *Parse::char_(letterDigitPredicate, "identifier"sv) >>
    [](std::string& result, const std::tuple<char, std::string>& value) static {
        result = std::get<0>(value) + std::get<1>(value);
    };
} // namespace

TEST(ParserTests, Identifier) {
    const auto result = identParser.parse("variableName123_xxx"sv);
    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), "variableName123_xxx"sv);
}

TEST(ParserTests, IdentifierInvalidInput) {
    const auto result = identParser.parse("123variable"sv);
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

TEST(ParserTests, PairOfIntegers) {
    constexpr auto parser = '(' >> Parse::integer() >> ';' >> Parse::integer() >> ')';

    const auto result = parser.parse("(-4;56)");
    ASSERT_TRUE(result);

    const auto& value = result.value();
    EXPECT_EQ(std::get<1>(value), -4);
    EXPECT_EQ(std::get<3>(value), 56);
}

TEST(ParserTests, ListOfIntegers) {
    constexpr auto parser =
        ~Parse::char_('(') >>
        Parse::integer().seq(*~Parse::ws() >> ~Parse::char_(';') >> *~Parse::ws()) >>
        ~Parse::char_(')');

    const auto result = parser.parse("(-4 ;56\t;3\r\n;  -5; 0; 1)");
    ASSERT_TRUE(result);

    const std::vector<int>& ints = result.value();
    ASSERT_EQ(ints.size(), 6);
    EXPECT_EQ(ints[0], -4);
    EXPECT_EQ(ints[1], 56);
    EXPECT_EQ(ints[2], 3);
    EXPECT_EQ(ints[3], -5);
    EXPECT_EQ(ints[4], 0);
    EXPECT_EQ(ints[5], 1);
}
