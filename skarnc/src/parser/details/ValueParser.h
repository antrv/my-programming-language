#pragma once

#include "ParserContext.h"

namespace skarn::parser {

template <details::Parser Parser, class Value>
class ValueParser final {
    Parser parser_;
    Value value_;

public:
    using ParserType = ValueParser;
    using InputType = Parser::InputType;
    using ValueType = Value;

    explicit constexpr ValueParser(Parser parser, Value value) noexcept
        : parser_ {std::move(parser)}
        , value_ {std::move(value)} {
    }

    bool parse(ParserContext<InputType>& ctx, Value& value) const
    requires (!std::is_same_v<Value, NoValueType>) {
        if (parser_.parse(ctx)) {
            value = value_;
            return true;
        }

        return false;
    }

    bool parse(ParserContext<InputType>& ctx) const {
        return parser_.parse(ctx);
    }
};

} // namespace skarn::parser
