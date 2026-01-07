#pragma once

#include "TypeTraits.h"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace skarn::ast {

struct Expression;

enum class UnaryOp {
    Plus,
    Minus,
};

struct UnaryExpression {
    std::unique_ptr<Expression> arg;
    UnaryOp op;
};

enum class BinaryOp {
    Add,
    Subtract,
    Multiply,
    Divide,
    Equal,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
};

struct BinaryExpression {
    std::vector<Expression> args;
    BinaryOp op;
};

struct VariableExpression {
    std::string name;
};

struct ConstantExpression {
    int value;
};

struct FunctionCallExpression {
    std::vector<Expression> args;
    std::string name;
};

struct Expression {
    std::variant<ConstantExpression, VariableExpression, UnaryExpression, BinaryExpression, FunctionCallExpression> value;

    /*implicit*/ Expression() noexcept = default;

    template <OneOf<ConstantExpression, VariableExpression, UnaryExpression, BinaryExpression, FunctionCallExpression> Arg>
    /*implicit*/ Expression(Arg&& arg) noexcept
        : value {std::forward<Arg>(arg)} {
    }

    static VariableExpression variable(std::string name) {
        return VariableExpression {std::move(name)};
    }

    static ConstantExpression constant(const int value) {
        return ConstantExpression {value};
    }

    template <OneOf<Expression, ConstantExpression, VariableExpression, UnaryExpression, BinaryExpression, FunctionCallExpression> Arg>
    static UnaryExpression unary(const UnaryOp op, Arg&& arg) {
        std::unique_ptr<Expression> arg_ptr = std::make_unique<Expression>(std::forward<Arg>(arg));
        return UnaryExpression {std::move(arg_ptr), op};
    }

    template <OneOf<Expression, ConstantExpression, VariableExpression, UnaryExpression, BinaryExpression, FunctionCallExpression>...Args>
    static BinaryExpression binary(const BinaryOp op, Args&&...args) {
        std::vector<Expression> args_vector;
        ((args_vector.push_back(Expression {std::forward<Args>(args)})), ...);
        return BinaryExpression {std::move(args_vector), op};
    }

    template <OneOf<Expression, ConstantExpression, VariableExpression, UnaryExpression, BinaryExpression, FunctionCallExpression>...Args>
    static FunctionCallExpression function(std::string name, Args&&...args) {
        std::vector<Expression> args_vector;
        ((args_vector.push_back(Expression {std::forward<Args>(args)})), ...);
        return FunctionCallExpression {std::move(args_vector), std::move(name)};
    }

    static FunctionCallExpression function(std::string name, std::vector<Expression> args) {
        return FunctionCallExpression {std::move(args), std::move(name)};
    }
};

bool operator ==(const Expression& lhs, const Expression& rhs) noexcept;

inline bool operator ==(const ConstantExpression& lhs, const ConstantExpression& rhs) noexcept {
    return lhs.value == rhs.value;
}

inline bool operator ==(const VariableExpression& lhs, const VariableExpression& rhs) noexcept {
    return lhs.name == rhs.name;
}

inline bool operator ==(const UnaryExpression& lhs, const UnaryExpression& rhs) noexcept {
    return lhs.op == rhs.op && (lhs.arg.get() == rhs.arg.get() || lhs.arg == nullptr || *lhs.arg == *rhs.arg);
}

inline bool operator ==(const BinaryExpression& lhs, const BinaryExpression& rhs) noexcept {
    if (lhs.op != rhs.op || lhs.args.size() != rhs.args.size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.args.size(); ++i) {
        if (lhs.args[i] != rhs.args[i]) {
            return false;
        }
    }

    return true;
}

inline bool operator ==(const FunctionCallExpression& lhs, const FunctionCallExpression& rhs) noexcept {
    if (lhs.name != rhs.name || lhs.args.size() != rhs.args.size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.args.size(); ++i) {
        if (lhs.args[i] != rhs.args[i]) {
            return false;
        }
    }

    return true;
}

inline bool operator ==(const Expression& lhs, const Expression& rhs) noexcept {
    return std::visit([]<class T, class U>(const T& lhs_expr, const U& rhs_expr) noexcept {
        if constexpr (std::is_same_v<T, U>) {
            return lhs_expr == rhs_expr;
        }
        else {
            return false;
        }
    }, lhs.value, rhs.value);
}

std::string to_string(const Expression& expr);

inline std::string to_string(const ConstantExpression& expr) {
    return std::to_string(expr.value);
}

inline std::string to_string(const VariableExpression& expr) {
    return expr.name;
}

constexpr std::string_view to_string(const UnaryOp op) noexcept {
    using namespace std::string_view_literals;
    switch (op) {
        case UnaryOp::Plus:
            return "+"sv;
        case UnaryOp::Minus:
            return "-"sv;
        default:
            return "?"sv;
    }
}

inline std::string to_string(const UnaryExpression& expr) {
    return std::format("{}{}", to_string(expr.op), expr.arg ? to_string(*expr.arg) : "null");
}

constexpr std::string_view to_string(const BinaryOp op) noexcept {
    using namespace std::string_view_literals;
    switch (op) {
        case BinaryOp::Add:
            return "+"sv;
        case BinaryOp::Subtract:
            return "-"sv;
        case BinaryOp::Multiply:
            return "*"sv;
        case BinaryOp::Divide:
            return "/"sv;
        case BinaryOp::Equal:
            return "=="sv;
        case BinaryOp::NotEqual:
            return "!="sv;
        case BinaryOp::LessThan:
            return "<"sv;
        case BinaryOp::LessThanOrEqual:
            return "<="sv;
        case BinaryOp::GreaterThan:
            return ">"sv;
        case BinaryOp::GreaterThanOrEqual:
            return ">="sv;
        default:
            return "?"sv;
    }
}

inline std::string to_string(const BinaryExpression& expr) {
    std::string result {"("};
    result += to_string(expr.args[0]);
    for (size_t i = 1; i < expr.args.size(); ++i) {
        result += std::format(" {} {}", to_string(expr.op), to_string(expr.args[i]));
    }

    result += ')';
    return result;
}

inline std::string to_string(const FunctionCallExpression& expr) {
    std::string result = expr.name;
    result += '(';
    result += to_string(expr.args[0]);
    for (size_t i = 1; i < expr.args.size(); ++i) {
        result += std::format(", {}", to_string(expr.args[i]));
    }

    result += ')';
    return result;
}

inline std::string to_string(const Expression& expr) {
    return std::visit([](const auto& ex) {
        return to_string(ex);
    }, expr.value);
}

} // namespace skarn::ast
