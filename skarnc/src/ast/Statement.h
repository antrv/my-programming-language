#pragma once

#include "Expression.h"
#include "Types.h"

namespace skarn::ast {

struct Statement;

struct VariableDeclarationStatement {
    std::string name;
    TypeInfo type;
    Expression initializer;
};

struct VariableAssignmentStatement {
    std::string name;
    TypeInfo type;
    Expression expression;
};

struct WhileStatement {
    Expression condition;
    std::vector<Statement> statements;
};

struct ReturnStatement {
    Expression expression;
};

struct Statement {
    std::variant<VariableDeclarationStatement, VariableAssignmentStatement, WhileStatement, ReturnStatement> value;

    Statement() = default;

    template <OneOf<VariableDeclarationStatement, VariableAssignmentStatement, WhileStatement, ReturnStatement> Arg>
    /* implicit */ Statement(Arg&& arg) noexcept
        : value {std::forward<Arg>(arg)} {
    }

    static VariableDeclarationStatement variableDeclaration(std::string name, Expression initializer) {
        return VariableDeclarationStatement {std::move(name), {}, std::move(initializer)};
    }

    static VariableAssignmentStatement variableAssignment(std::string name, Expression initializer) {
        return VariableAssignmentStatement {std::move(name), {}, std::move(initializer)};
    }

    static ReturnStatement returnStatement(Expression expression) {
        return ReturnStatement {std::move(expression)};
    }

    static WhileStatement whileStatement(Expression expression, std::vector<Statement> statements) {
        return WhileStatement {std::move(expression), std::move(statements)};
    }
};

} // namespace skarn::ast
