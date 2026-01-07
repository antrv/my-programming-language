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
    std::vector<std::unique_ptr<Statement>> statements;
};

} // namespace skarn::ast
