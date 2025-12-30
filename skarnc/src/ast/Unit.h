#pragma once

#include "Function.h"
#include "Import.h"

namespace skarn::ast {

struct Unit {
    std::string unitName;
    std::vector<Import> imports;
    std::vector<Function> functions;
};

} // namespace skarn::ast
