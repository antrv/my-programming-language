#pragma once

#include "ParserContext.h"

namespace skarn::parser::details {

/// Parser that does not generate a value.
template <Parser Parser>
class IgnoreParser final {
    Parser parser_;

public:
    using ParserType = IgnoreParser;
    using InputType = Parser::InputType;
    using ValueType = NoValueType;

    explicit constexpr IgnoreParser(Parser parser) noexcept
        : parser_ {std::move(parser)} {
    }

    bool parse(ParserContext<char>& ctx, [[maybe_unused]] ValueType& value) const {
        return parser_.parse(ctx);
    }

    bool parse(ParserContext<char>& ctx) const {
        return parser_.parse(ctx);
    }
};

} // namespace skarn::parser::details
