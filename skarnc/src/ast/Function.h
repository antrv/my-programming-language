#pragma once

#include "Statement.h"

namespace skarn::ast {

struct FunctionArgument {
    std::string name;
    TypeInfo type;
};

struct Function {
    std::string name;
    std::vector<FunctionArgument> arguments;
    std::vector<Statement> statements;
    std::optional<Expression> lastExpression;
};

} // namespace skarn::ast
