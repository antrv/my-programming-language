#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace skarn::ast {

struct Expression;

struct UnaryExpression {
    std::unique_ptr<Expression> arg;
    char op;
};

struct BinaryExpression {
    std::vector<std::unique_ptr<Expression>> args;
    char op;
};

struct VariableExpression {
    std::string name;
};

struct ConstantExpression {
    int value;
};

struct FunctionCallExpression {
    std::vector<std::unique_ptr<Expression>> args;
    std::string name;
};

struct Expression {
    std::variant<UnaryExpression, BinaryExpression, VariableExpression, ConstantExpression, FunctionCallExpression> value;
};

} // namespace skarn::ast
