#pragma once

#include "ParserContext.h"

namespace skarn::parser {

template <class Value>
class ConstantParser final {
    Value value_;

public:
    using ParserType = ConstantParser;
    using InputType = AnyInputType;
    using ValueType = Value;

    explicit constexpr ConstantParser(Value value) noexcept
        : value_ {std::move(value)} {
    }

    template <class Elem>
    bool parse([[maybe_unused]] ParserContext<Elem>& ctx, Value& value) const
    requires (!std::is_same_v<Value, NoValueType>) {
        value = value_;
        return true;
    }

    template <class Elem>
    bool parse([[maybe_unused]] ParserContext<Elem>& ctx) const {
        std::ignore = this;
        return true;
    }
};

} // namespace skarn::parser
