#pragma once

#include "ast/Unit.h"
#include "ast/Expression.h"
#include "parser/Parser.h"

namespace skarn::ast {

consteval auto create_parser() noexcept {
    using namespace std::string_view_literals;
    using namespace skarn::parser;

    constexpr auto ws_many = *~Parse::ws();
    constexpr auto ws_at_least_one = +~Parse::ws();

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
        [](std::string& result, const std::tuple<char, std::string>& value) static {
            result = std::get<0>(value) + std::get<1>(value);
        };

    constexpr auto expressionRef = Parse::ref<Expression>();

    constexpr auto constantExpression = Parse::integer<int>() >> [](Expression& expression, const int& value) static {
        expression.value = ConstantExpression {value};
    };

    constexpr auto variableExpression = ident >> [](Expression& expression, std::string& value) static {
        expression.value = VariableExpression {std::move(value)};
    };

    constexpr auto bracketExpression = ~Parse::char_('(') >> ws_many >> expressionRef >> ws_many >> ~Parse::char_(')');
    constexpr auto simpleExpression = *sum_op >> (constantExpression || variableExpression || bracketExpression);
    constexpr auto productExpression = simpleExpression >> ws_many >> *(product_op >> ws_many >> simpleExpression);
    constexpr auto sumExpression = productExpression >> ws_many >> *(sum_op >> ws_many >> productExpression);
    constexpr auto expression = sumExpression;
    expressionRef.assign(expression);

    constexpr auto variableDeclaration =
        ~Parse::literal("let"sv) >> ws_at_least_one >> ident >> ws_many >>
        ~Parse::char_('=') >> ws_many >> expression >>
        ~Parse::char_(';');

    constexpr auto variableAssignment =
        ident >> ws_many >> ~Parse::char_('=') >> ws_many >> expression >>
        ~Parse::char_(';');

    constexpr auto whileLoop =
        ~Parse::literal("while"sv) >> ws_at_least_one >> expression >> ws_many >>
        ~Parse::char_('{') >> ws_many >> statement >> ws_many >> ~Parse::char_('}');

    constexpr auto statement = variableDeclaration || variableAssignment || whileLoop;

    constexpr auto function =
        ~Parse::literal("fn"sv) >> ws_at_least_one >> ident >> ws_many >>
        ~Parse::char_('(') >> ws_many >> ident.seq(ws_many >> ',' >> ws_many) >> ws_many >> ~Parse::char_(')') >> ws_many >>
        ~Parse::char_('{') >> ws_many >> *(statement >> ws_many) >> expression.optional() >> ws_many >> ~Parse::char_('}');

    constexpr auto unitParser = *function.wrap(ws_many) >>
        [](Unit& unit, std::vector<Function>& functions) static noexcept {
            unit.functions = std::move(functions);
        };

    return unitParser;
}

} // namespace skarn::ast
