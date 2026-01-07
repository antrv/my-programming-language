#pragma once

#include "ParserContext.h"

namespace skarn::parser {

/// Parser that does not generate a value.
template <details::Parser Parser>
class IgnoreParser final {
    Parser parser_;

public:
    using ParserType = IgnoreParser;
    using InputType = Parser::InputType;
    using ValueType = NoValueType;

    explicit constexpr IgnoreParser(Parser parser) noexcept
        : parser_ {std::move(parser)} {
    }

    bool parse(ParserContext<InputType>& ctx, [[maybe_unused]] ValueType& value) const
    requires (!std::is_same_v<ValueType, NoValueType>) {
        return parser_.parse(ctx);
    }

    bool parse(ParserContext<InputType>& ctx) const {
        return parser_.parse(ctx);
    }
};

template <details::Parser Parser>
constexpr auto makeIgnoreParser(Parser parser) noexcept {
    if constexpr (SpecializationOf<Parser, IgnoreParser>) {
        return parser;
    }
    else {
        return IgnoreParser<Parser> {std::move(parser)};
    }
}

} // namespace skarn::parser
