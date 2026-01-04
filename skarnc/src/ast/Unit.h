#pragma once

#include "Function.h"

namespace skarn::ast {

struct Unit {
    std::string unitName;
    std::vector<Function> functions;
};

} // namespace skarn::ast
