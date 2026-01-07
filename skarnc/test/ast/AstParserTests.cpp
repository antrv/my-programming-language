#include "parser/Parser.h"
#include "ast/Unit.h"
#include <algorithm>
#include <gtest/gtest.h>

using namespace std::string_view_literals;
using namespace skarn::ast;
using namespace skarn::parser;

TEST(AstParserTests, ParseSimpleExpression) {
    constexpr auto ws_many = *~Parse::ws();

    constexpr auto product_op = Parse::char_([](const char c) static noexcept {
            return c == '*' || c == '/';
        });

    constexpr auto sum_op = Parse::char_([](const char c) static noexcept {
            return c == '+' || c == '-';
        });

    constexpr auto ident = Parse::char_([](const char c) static noexcept {
            return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
        }, "identifier"sv) >>
        *Parse::char_([](const char c) static noexcept {
            return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_';
        }, "identifier"sv) >>
        [](std::string& result, std::tuple<char, std::string>& value) static {
            result = std::get<0>(value) + std::move(std::get<1>(value));
        };

    constexpr auto expressionRef = Parse::ref<Expression>();

    constexpr auto constantExpression =
        Parse::integer<int>() >> [](Expression& expression, const int value) static {
            expression = Expression::constant(value);
        };

    constexpr auto variableExpression =
        ident >> [](Expression& expression, std::string& value) static {
            expression = Expression::variable(std::move(value));
        };

    constexpr auto bracketExpression =
        ~Parse::char_('(') >> ws_many >> expressionRef >> ws_many >> ~Parse::char_(')');

    constexpr auto simpleExpression = constantExpression || variableExpression || bracketExpression;

    constexpr auto unaryExpression = *(ws_many >> sum_op) >> ws_many >> simpleExpression >>
        [](Expression& expression, std::tuple<std::string, Expression>& value) static {
            const std::string& ops = std::get<0>(value);
            const size_t minusCount = std::ranges::count_if(ops, [](const char c) static noexcept {
                return c == '-';
            });

            if (minusCount % 2 == 0) {
                expression = std::move(std::get<1>(value));
            }
            else {
                expression = Expression::unary('-', std::move(std::get<1>(value)));
            }
        };

    constexpr auto productExpression =
        unaryExpression >> ws_many >> *(ws_many >> product_op >> ws_many >> unaryExpression) >>
        [](Expression& expression, std::tuple<Expression, std::vector<std::tuple<char, Expression>>>& value) {
            expression = std::move(std::get<0>(value));
            for (auto& [op, arg] : std::get<1>(value)) {
                expression = Expression::binary(op, std::move(expression), std::move(arg));
            }
        };

    constexpr auto expression =
        productExpression >> ws_many >> *(ws_many >> sum_op >> ws_many >> productExpression) >>
        [](Expression& expression, std::tuple<Expression, std::vector<std::tuple<char, Expression>>>& value) {
            expression = std::move(std::get<0>(value));
            for (auto& [op, arg] : std::get<1>(value)) {
                expression = Expression::binary(op, std::move(expression), std::move(arg));
            }
    };

    expressionRef.assign(expression);

    auto result = expression.parse("a + b * (2 + c / 4) + 5 * -7");
    ASSERT_TRUE(result);

    const Expression& value = result.value();
    const Expression expected =
        Expression::binary('+',
            Expression::binary('+',
                Expression::variable("a"),
                Expression::binary('*',
                    Expression::variable("b"),
                    Expression::binary('+',
                        Expression::constant(2),
                        Expression::binary('/',
                            Expression::variable("c"),
                            Expression::constant(4))))),
            Expression::binary('*',
                Expression::constant(5),
                Expression::unary('-',
                    Expression::constant(7))));



    EXPECT_EQ(to_string(value), to_string(expected));
    EXPECT_TRUE(value == expected);
    EXPECT_EQ(value, expected);
}
