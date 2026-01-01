#pragma once

#include "ParserContext.h"

namespace skarn::parser::details {

template <class Value>
class ValueParser final {
    Value value_;

public:
    using ParserType = ValueParser;
    using InputType = AnyInputType;
    using ValueType = Value;

    explicit constexpr ValueParser(Value value) noexcept
        : value_ {std::move(value)} {
    }

    template <class Elem>
    bool parse([[maybe_unused]] ParserContext<Elem>& ctx, Value& value) const {
        value = value_;
        return true;
    }
};

} // namespace skarn::parser::details
