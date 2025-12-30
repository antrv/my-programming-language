#pragma once

#include <memory>
#include <string>
#include <vector>

namespace skarn::ast {

enum class TypeKind {
    Unknown,
    Int,
    Bool,
};

struct TypeInfo {
    TypeKind kind {TypeKind::Unknown};
};

struct FunctionArgument {
    std::string name;
    TypeInfo type;
};

struct Statement {
    virtual ~Statement() = default;

protected:
    Statement() = default;
};

struct Expression {
    virtual ~Expression() = default;

protected:
    Expression() = default;
};

struct ConstantExpression : Expression {
    int value;
};

struct VariableExpression : Expression {
    std::string name;
};

struct VariableDeclStatement : Statement {
    std::string name;
    TypeInfo type;
    std::unique_ptr<Expression> initializer;
};

struct WhileStatement : Statement {
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> statements;
};

struct Function {
    std::string name;
    std::vector<FunctionArgument> arguments;
    std::vector<std::unique_ptr<Statement>> statements;
};

} // namespace skarn::ast
