#pragma once

#include "ParserContext.h"
#include "InvocableTraits.h"

namespace skarn::parser::details {

template <class T, class Value>
concept TransformInvocable = std::is_same_v<invocable_result_t<T>, void> &&
    invocable_args_t<T>::size == 2 &&
    std::is_lvalue_reference_v<typename invocable_args_t<T>::template element_t<0>> &&
    !std::is_const_v<std::remove_reference_t<typename invocable_args_t<T>::template element_t<0>>> &&
    std::is_same_v<std::remove_cvref_t<typename invocable_args_t<T>::template element_t<1>>, Value>;

template <Parser Parser, TransformInvocable<typename Parser::ValueType> Transform>
class TransformParser final {
    Parser parser_;
    Transform transform_;

public:
    using ParserType = TransformParser;
    using InputType = Parser::InputType;
    using ValueType = std::remove_reference_t<typename invocable_args_t<Transform>::template element_t<0>>;

    explicit constexpr TransformParser(Parser parser, Transform transform) noexcept
        : parser_ {std::move(parser)}
        , transform_ {std::move(transform)} {
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const {
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

} // namespace skarn::parser::details
