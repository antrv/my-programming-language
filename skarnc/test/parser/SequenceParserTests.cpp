#include <gtest/gtest.h>
#include "parser/details/CharParser.h"
#include "parser/details/SequenceParser.h"

using namespace std::string_view_literals;
using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(SequenceParserTests, Success) {
    constexpr SequenceParser parser {CharParser {'a'}};

    constexpr std::string_view input {"aaaaabbb"sv};
    ParserContext<char> ctx {input};
    std::vector<char> value;
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {value}, "aaaaa"sv);
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_EQ(std::string_view {ctx.input()}, "bbb"sv);
}

TEST(SequenceParserTests, EmptyInput) {
    constexpr SequenceParser parser {CharParser {'a'}};

    constexpr std::string_view input;
    ParserContext<char> ctx {input};
    std::vector<char> value;
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_EQ(std::string_view {ctx.input()}, input);
}

TEST(SequenceParserTests, AtLeastOnceEmptyInput) {
    constexpr SequenceParser<CharParser, 1> parser {CharParser {'a'}};

    constexpr std::string_view input;
    ParserContext<char> ctx {input};
    std::vector<char> value;
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0001);
    EXPECT_EQ(messages[0].expected, "'a'"sv);
    EXPECT_EQ(messages[0].offset, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}

TEST(SequenceParserTests, InvalidInput) {
    constexpr SequenceParser parser {CharParser {'a'}};

    constexpr std::string_view input {"b"sv};
    ParserContext<char> ctx {input};
    std::vector<char> value;
    ASSERT_TRUE(parser.parse(ctx, value));
    EXPECT_TRUE(ctx.messages().empty());
    EXPECT_EQ(std::string_view {ctx.input()}, input);
}

TEST(SequenceParserTests, AtLeastOnceInvalidInput) {
    constexpr SequenceParser<CharParser, 1> parser {CharParser {'a'}};

    constexpr std::string_view input {"b"sv};
    ParserContext<char> ctx {input};
    std::vector<char> value;
    ASSERT_FALSE(parser.parse(ctx, value));
    EXPECT_EQ(std::string_view {ctx.input()}, input);

    const auto& messages = ctx.messages();
    ASSERT_EQ(messages.size(), 1);
    EXPECT_EQ(messages[0].level, ParserMsgLevel::Error);
    EXPECT_EQ(messages[0].code, ParserMsgCode::C0002);
    EXPECT_EQ(messages[0].expected, "'a'"sv);
    EXPECT_EQ(messages[0].offset, 0);
    EXPECT_EQ(messages[0].line, 1U);
    EXPECT_EQ(messages[0].column, 1U);
}
