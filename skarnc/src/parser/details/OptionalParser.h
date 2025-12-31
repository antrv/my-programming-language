#pragma once

#include "ParserContext.h"
#include <optional>

namespace skarn::parser::details {

template <IsParser Parser>
class OptionalParser final {
    Parser parser_;

public:
    using ParserType = OptionalParser;
    using ValueType = std::optional<typename Parser::ValueType>;
    using InputType = Parser::InputType;

    explicit constexpr OptionalParser(Parser parser) noexcept
        : parser_ {std::move(parser)} {
    }

    [[nodiscard]] constexpr const Parser& parser() const noexcept {
        return parser_;
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const {
        value.emplace();
        if (!parser_.parse(ctx, *value)) {
            value.reset();
        }

        return true;
    }
};

} // namespace skarn::parser::details
