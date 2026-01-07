#pragma once

namespace skarn::ast {

enum class TypeKind {
    Unknown,
    Int,
    Bool,
};

struct TypeInfo {
    TypeKind kind {TypeKind::Unknown};
};

} // namespace skarn::ast
