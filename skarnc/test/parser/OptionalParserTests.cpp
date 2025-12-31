#include <gtest/gtest.h>
#include "parser/details/CharParser.h"
#include "parser/details/OptionalParser.h"

using namespace skarn::parser;
using namespace skarn::parser::details;

TEST(OptionalParserTests, OptionalParser)
{
    constexpr auto parser = OptionalParser {CharParser {'a'}};

    ParserContext<char> ctx1 {"a"};
    std::optional<char> value1;
    ASSERT_TRUE(parser.parse(ctx1, value1));
    EXPECT_EQ(value1, 'a');
    EXPECT_EQ(ctx1.messages().size(), 0);

    ParserContext<char> ctx2 {""};
    std::optional<char> value2;
    ASSERT_TRUE(parser.parse(ctx2, value2));
    EXPECT_EQ(value2, std::nullopt);
    EXPECT_EQ(ctx2.messages().size(), 0);

    ParserContext<char> ctx3 {"b"};
    std::optional<char> value3;
    ASSERT_TRUE(parser.parse(ctx3, value3));
    EXPECT_EQ(value3, std::nullopt);
    EXPECT_EQ(ctx3.messages().size(), 0);
}
