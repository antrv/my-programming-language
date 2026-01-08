#include "parser/Parser.h"
#include "ast/Unit.h"
#include <algorithm>
#include <gtest/gtest.h>

#include "TypeName.h"

using namespace std::string_view_literals;
using namespace skarn::ast;
using namespace skarn::parser;

namespace {
constexpr auto ws_many = *~Parse::ws();
constexpr auto ws_at_least_once = +~Parse::ws();

constexpr auto unary_op =
    Parse::char_('+').value(UnaryOp::Plus) ||
    Parse::char_('-').value(UnaryOp::Minus);

constexpr auto product_op =
    Parse::char_('*').value(BinaryOp::Multiply) ||
    Parse::char_('/').value(BinaryOp::Divide);

constexpr auto sum_op =
    Parse::char_('+').value(BinaryOp::Add) ||
    Parse::char_('-').value(BinaryOp::Subtract);

constexpr auto compare_op =
    Parse::literal("!="sv).value(BinaryOp::NotEqual) ||
    Parse::literal("=="sv).value(BinaryOp::Equal) ||
    Parse::literal("<="sv).value(BinaryOp::LessThanOrEqual) ||
    Parse::literal("<"sv).value(BinaryOp::LessThan) ||
    Parse::literal(">="sv).value(BinaryOp::GreaterThanOrEqual) ||
    Parse::literal(">"sv).value(BinaryOp::GreaterThan);

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
    Parse::integer<int>() >>
    [](Expression& result, const int value) static {
        result = Expression::constant(value);
    };

constexpr auto variableExpression =
    ident >> [](Expression& result, std::string& value) static {
        result = Expression::variable(std::move(value));
    };

constexpr auto bracketExpression =
    ~Parse::char_('(') >> ws_many >> expressionRef >> ws_many >> ~Parse::char_(')');

constexpr auto functionCallExpression =
    ident >> ws_many >> ~Parse::char_('(') >> ws_many >>
    (expressionRef >> *(ws_many >> ~Parse::char_(',') >> ws_many >> expressionRef)).optional() >> ws_many >>
    ~Parse::char_(')') >>
    [](Expression& result, std::tuple<std::string, std::optional<std::tuple<Expression, std::vector<Expression>>>>& value) {
        std::vector<Expression> args;
        if (std::optional<std::tuple<Expression, std::vector<Expression>>>& args_value = std::get<1>(value)) {
            args.push_back(std::move(std::get<0>(args_value.value())));
            for (Expression& arg : std::get<1>(args_value.value())) {
                args.push_back(std::move(arg));
            }
        }

        result = Expression::function(std::move(std::get<0>(value)), std::move(args));
    };

constexpr auto simpleExpression = constantExpression || functionCallExpression || variableExpression || bracketExpression;

constexpr auto unaryExpression = *(ws_many >> unary_op) >> ws_many >> simpleExpression >>
    [](Expression& result, std::tuple<std::vector<UnaryOp>, Expression>& value) static {
        const std::vector<UnaryOp>& ops = std::get<0>(value);
        const size_t minusCount = std::ranges::count_if(ops, [](const UnaryOp op) static noexcept {
            return op == UnaryOp::Minus;
        });

        if (minusCount % 2 == 0) {
            result = std::move(std::get<1>(value));
        }
        else {
            result = Expression::unary(UnaryOp::Minus, std::move(std::get<1>(value)));
        }
    };

constexpr auto productExpression =
    unaryExpression >> ws_many >> *(ws_many >> product_op >> ws_many >> unaryExpression) >>
    [](Expression& result, std::tuple<Expression, std::vector<std::tuple<BinaryOp, Expression>>>& value) static {
        result = std::move(std::get<0>(value));
        for (auto& [op, arg] : std::get<1>(value)) {
            result = Expression::binary(op, std::move(result), std::move(arg));
        }
    };

constexpr auto sumExpression =
    productExpression >> ws_many >> *(ws_many >> sum_op >> ws_many >> productExpression) >>
    [](Expression& result, std::tuple<Expression, std::vector<std::tuple<BinaryOp, Expression>>>& value) static {
        result = std::move(std::get<0>(value));
        for (auto& [op, arg] : std::get<1>(value)) {
            result = Expression::binary(op, std::move(result), std::move(arg));
        }
    };

constexpr auto expression =
    (sumExpression >> ws_many >> *(ws_many >> compare_op >> ws_many >> sumExpression) >>
    [](Expression& result, std::tuple<Expression, std::vector<std::tuple<BinaryOp, Expression>>>& value) static {
        result = std::move(std::get<0>(value));
        for (auto& [op, arg] : std::get<1>(value)) {
            result = Expression::binary(op, std::move(result), std::move(arg));
        }
    }).expected("expression"sv);

constexpr auto statementRef = Parse::ref<Statement>();

constexpr auto variableDeclaration =
    ~Parse::literal("let"sv) >> ws_at_least_once >> ident >> ws_many >>
    ~Parse::char_('=') >> ws_many >> expression >> ws_many >> ~Parse::char_(';') >>
    [](Statement& result, std::tuple<std::string, Expression>& value) static {
        result = Statement::variableDeclaration(std::move(std::get<0>(value)), std::move(std::get<1>(value)));
    };

constexpr auto variableAssignment =
    ident >> ws_many >> ~Parse::char_('=') >> ws_many >> expression >> ws_many >> ~Parse::char_(';') >>
    [](Statement& result, std::tuple<std::string, Expression>& value) static {
        result = Statement::variableAssignment(std::move(std::get<0>(value)), std::move(std::get<1>(value)));
    };

constexpr auto returnStatement =
    ~Parse::literal("return"sv) >> ws_many >> expression >> ws_many >> ~Parse::char_(';') >>
    [](Statement& result, Expression& value) static {
        result = Statement::returnStatement(std::move(value));
    };

constexpr auto whileStatement =
    ~Parse::literal("while"sv) >> ws_at_least_once >> expression >> ws_many >>
    ~Parse::char_('{') >> ws_many >> *(statementRef >> ws_many) >>
    ~Parse::char_('}') >>
    [](Statement& result, std::tuple<Expression, std::vector<Statement>>& value) static {
        result = Statement::whileStatement(std::move(std::get<0>(value)), std::move(std::get<1>(value)));
    };

constexpr auto statement = returnStatement || variableDeclaration || variableAssignment || whileStatement;

constexpr auto function =
    ~Parse::literal("fn"sv) >> ws_at_least_once >> ident >> ws_many >>
    ~Parse::char_('(') >> ws_many >> ident.seq(ws_many >> ',' >> ws_many) >> ws_many >> ~Parse::char_(')') >> ws_many >>
    ~Parse::char_('{') >> ws_many >> *(statement >> ws_many) >> expression.optional() >> ws_many >> ~Parse::char_('}') >>
    [](Function& result, std::tuple<std::string, std::vector<std::string>, std::vector<Statement>, std::optional<Expression>>& value) static {
        result.name = std::get<0>(value);
        for (std::string& arg : std::get<1>(value)) {
            result.arguments.push_back(FunctionArgument {
                .name = std::move(arg),
            });
        }

        result.statements = std::move(std::get<2>(value));
        result.lastExpression = std::move(std::get<3>(value));
    };

} // namespace

TEST(AstParserTests, ParseSimpleExpression) {
    expressionRef.assign(expression);

    auto result = expression.parse("a + b * (2 + c / f(4, d, g(1))) + 5 * -7 >= 1");
    ASSERT_TRUE(result);

    const Expression& value = result.value();
    const Expression expected =
        Expression::binary(BinaryOp::GreaterThanOrEqual,
            Expression::binary(BinaryOp::Add,
                Expression::binary(BinaryOp::Add,
                    Expression::variable("a"),
                    Expression::binary(BinaryOp::Multiply,
                        Expression::variable("b"),
                        Expression::binary(BinaryOp::Add,
                            Expression::constant(2),
                            Expression::binary(BinaryOp::Divide,
                                Expression::variable("c"),
                                Expression::function("f",
                                    Expression::constant(4),
                                    Expression::variable("d"),
                                    Expression::function("g",
                                        Expression::constant(1))))))),
                Expression::binary(BinaryOp::Multiply,
                    Expression::constant(5),
                    Expression::unary(UnaryOp::Minus,
                        Expression::constant(7)))),
            Expression::constant(1));

    EXPECT_EQ(to_string(value), to_string(expected));
    EXPECT_EQ(value, expected);
}

TEST(AstParserTests, VariableDeclaration) {
    expressionRef.assign(expression);

    const auto result = variableDeclaration.parse("let a = 1 + 2;");
    ASSERT_TRUE(result);

    const Statement& stmt = result.value();
    const auto& varDecl = std::get<VariableDeclarationStatement>(stmt.value);
    const Expression& initializer = varDecl.initializer;
    EXPECT_EQ(varDecl.name, "a"sv);
    const Expression expected =
        Expression::binary(BinaryOp::Add,
            Expression::constant(1),
            Expression::constant(2));

    EXPECT_EQ(initializer, expected);
}

TEST(AstParserTests, VariableAssignment) {
    expressionRef.assign(expression);

    const auto result = variableAssignment.parse("a = a + 2;");
    ASSERT_TRUE(result);

    const Statement& stmt = result.value();
    const auto& varAssign = std::get<VariableAssignmentStatement>(stmt.value);
    const Expression& expr = varAssign.expression;
    EXPECT_EQ(varAssign.name, "a"sv);
    const Expression expected =
        Expression::binary(BinaryOp::Add,
            Expression::variable("a"),
            Expression::constant(2));

    EXPECT_EQ(expr, expected);
}

TEST(AstParserTests, ReturnStatement) {
    expressionRef.assign(expression);

    const auto result = returnStatement.parse("return (x / 2);");
    ASSERT_TRUE(result);

    const Statement& stmt = result.value();
    const auto& returnSt = std::get<ReturnStatement>(stmt.value);
    const Expression& expr = returnSt.expression;
    const Expression expected =
        Expression::binary(BinaryOp::Divide,
            Expression::variable("x"),
            Expression::constant(2));

    EXPECT_EQ(expr, expected);
}

TEST(AstParserTests, WhileStatement) {
    expressionRef.assign(expression);
    statementRef.assign(statement);

    constexpr std::string_view text {
        "while i < 10 {"
        "    a = a / 2;"
        "    i = i + 1;"
        "}"
    };

    const auto result = whileStatement.parse(text);
    ASSERT_TRUE(result);

    const Statement& stmt = result.value();
    const auto& whileSt = std::get<WhileStatement>(stmt.value);
    const Expression& expr = whileSt.condition;
    const Expression expected =
        Expression::binary(BinaryOp::LessThan,
            Expression::variable("i"),
            Expression::constant(10));

    EXPECT_EQ(expr, expected);

    const auto& body = whileSt.statements;
    ASSERT_EQ(body.size(), 2U);
    const auto& first = std::get<VariableAssignmentStatement>(body[0].value);
    const auto& second = std::get<VariableAssignmentStatement>(body[1].value);

    EXPECT_EQ(first.name, "a"sv);
    const Expression firstExpr =
        Expression::binary(BinaryOp::Divide,
            Expression::variable("a"),
            Expression::constant(2));

    EXPECT_EQ(first.expression, firstExpr);

    EXPECT_EQ(second.name, "i"sv);
    const Expression secondExpr =
        Expression::binary(BinaryOp::Add,
            Expression::variable("i"),
            Expression::constant(1));

    EXPECT_EQ(second.expression, secondExpr);
}

TEST(AstParserTests, Function1) {
    expressionRef.assign(expression);
    statementRef.assign(statement);

    constexpr std::string_view text {
        "fn add(a, b) {"
        "    return a + b;"
        "}"
    };

    const auto result = function.parse(text);
    ASSERT_TRUE(result);
}

TEST(AstParserTests, Function2) {
    expressionRef.assign(expression);
    statementRef.assign(statement);

    constexpr std::string_view text {
        "fn add_numbers(a, b) {"
        "    while a < 10 {"
        "        a = a + 1;"
        "    }"
        "    a + b"
        "}"
    };

    const auto result = function.parse(text);
    ASSERT_TRUE(result);

    const auto& func = result.value();
    EXPECT_EQ(func.name, "add_numbers"sv);
    EXPECT_EQ(func.arguments.size(), 2);
    EXPECT_EQ(func.statements.size(), 1);
    EXPECT_TRUE(func.lastExpression.has_value());
}
