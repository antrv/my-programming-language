#pragma once

#include "ParserContext.h"

namespace skarn::parser {

namespace details {
template <class T, class Value>
struct TransformInvocableResult;

template <class Type, class Result, class Arg, class Value>
struct TransformInvocableResult<void(Type::*)(Result&, Arg), Value> : std::type_identity<Result> {
};

template <class Type, class Result, class Arg, class Value>
struct TransformInvocableResult<void(Type::*)(Result&, Arg) noexcept, Value> : std::type_identity<Result> {
};

template <class Type, class Result, class Arg, class Value>
struct TransformInvocableResult<void(Type::*)(Result&, Arg) const, Value> : std::type_identity<Result> {
};

template <class Type, class Result, class Arg, class Value>
struct TransformInvocableResult<void(Type::*)(Result&, Arg) const noexcept, Value> : std::type_identity<Result> {
};

template <class Result, class Arg, class Value>
struct TransformInvocableResult<void(*)(Result&, Arg), Value> : std::type_identity<Result> {
};

template <class Result, class Arg, class Value>
struct TransformInvocableResult<void(*)(Result&, Arg) noexcept, Value> : std::type_identity<Result> {
};

template <class Result, class Arg, class Value>
struct TransformInvocableResult<void(Result&, Arg), Value> : std::type_identity<Result> {
};

template <class Result, class Arg, class Value>
struct TransformInvocableResult<void(Result&, Arg) noexcept, Value> : std::type_identity<Result> {
};

template <class F, class Value>
requires (requires { &F::operator(); })
struct TransformInvocableResult<F, Value> : TransformInvocableResult<decltype(&F::operator()), Value> {
};

template <class F, class Value>
requires (requires { &F::template operator()<Value>; })
struct TransformInvocableResult<F, Value> : TransformInvocableResult<decltype(&F::template operator()<Value>), Value> {
};

template <class T, class Value>
concept TransformInvocable =
    requires (const T& t, typename TransformInvocableResult<T, Value>::type& result, Value& value) {
    { t(result, value) } -> std::same_as<void>;
    };
} // namespace details

template <details::Parser Parser, details::TransformInvocable<typename Parser::ValueType> Transform>
class TransformParser final {
    Parser parser_;
    Transform transform_;

public:
    using ParserType = TransformParser;
    using InputType = Parser::InputType;
    using ValueType = details::TransformInvocableResult<Transform, typename Parser::ValueType>::type;

    explicit constexpr TransformParser(Parser parser, Transform transform) noexcept
        : parser_ {std::move(parser)}
        , transform_ {std::move(transform)} {
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const
    requires (!std::is_same_v<ValueType, NoValueType>) {
        if (typename Parser::ValueType val {};
            parser_.parse(ctx, val)) {
            transform_(value, val);
            return true;
        }

        return false;
    }

    bool parse(ParserContext<InputType>& ctx) const {
        return parser_.parse(ctx);
    }
};

} // namespace skarn::parser
