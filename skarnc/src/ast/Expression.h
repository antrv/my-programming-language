#pragma once

#include "TypeTraits.h"
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
    static UnaryExpression unary(const char op, Arg&& arg) {
        std::unique_ptr<Expression> arg_ptr = std::make_unique<Expression>(std::forward<Arg>(arg));
        return UnaryExpression {std::move(arg_ptr), op};
    }

    template <OneOf<Expression, ConstantExpression, VariableExpression, UnaryExpression, BinaryExpression, FunctionCallExpression>...Args>
    static BinaryExpression binary(const char op, Args&&...args) {
        std::vector<std::unique_ptr<Expression>> args_vector;
        (args_vector.push_back(std::make_unique<Expression>(std::forward<Args>(args))), ...);
        return BinaryExpression {std::move(args_vector), op};
    }

    template <OneOf<Expression, ConstantExpression, VariableExpression, UnaryExpression, BinaryExpression, FunctionCallExpression>...Args>
    static FunctionCallExpression function(std::string name, Args&&...args) {
        std::vector<std::unique_ptr<Expression>> args_vector;
        (args_vector.push_back(std::make_unique<Expression>(std::forward<Args>(args))), ...);
        return FunctionCallExpression {std::move(args_vector), std::move(name)};
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
        if (!(lhs.args[i].get() == rhs.args[i].get() || lhs.args[i] == nullptr || *lhs.args[i] == *rhs.args[i])) {
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
        if (!(lhs.args[i].get() == rhs.args[i].get() || lhs.args[i] == nullptr || *lhs.args[i] == *rhs.args[i])) {
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

inline std::string to_string(const UnaryExpression& expr) {
    return std::format("{}{}", expr.op, expr.arg ? to_string(*expr.arg) : "null");
}

inline std::string to_string(const BinaryExpression& expr) {
    std::string result {"("};
    result += to_string(*expr.args[0]);
    for (size_t i = 1; i < expr.args.size(); ++i) {
        result += std::format(" {} {}", expr.op, to_string(*expr.args[i]));
    }

    result += ')';
    return result;
}

inline std::string to_string(const FunctionCallExpression& expr) {
    std::string result = expr.name;
    result += '(';
    result += to_string(*expr.args[0]);
    for (size_t i = 1; i < expr.args.size(); ++i) {
        result += std::format(", {}", to_string(*expr.args[i]));
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
