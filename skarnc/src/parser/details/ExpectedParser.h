#pragma once

#include "ParserContext.h"

namespace skarn::parser::details {

/// Parser that replaces the error message.
template <Parser Parser>
class ExpectedParser final {
    Parser parser_;
    std::string_view what_;

public:
    using ParserType = ExpectedParser;
    using InputType = Parser::InputType;
    using ValueType = Parser::ValueType;

    explicit constexpr ExpectedParser(Parser parser, const std::string_view what) noexcept
        : parser_ {std::move(parser)}
        , what_{what} {
    }

    [[nodiscard]] constexpr const Parser& parser() const noexcept {
        return parser_;
    }

    [[nodiscard]] constexpr std::string_view what() const noexcept {
        return what_;
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const {
        if (!parser_.parse(ctx, value)) {
            if (ctx.report_messages()) {
                ctx.messages().back().expected = what_;
            }

            return false;
        }

        return true;
    }

    bool parse(ParserContext<InputType>& ctx) const {
        if (!parser_.parse(ctx)) {
            if (ctx.report_messages()) {
                ctx.messages().back().expected = what_;
            }

            return false;
        }

        return true;
    }
};

template <Parser Parser>
constexpr auto makeExpectedParser(Parser parser, const std::string_view what) noexcept {
    if constexpr (SpecializationOf<Parser, ExpectedParser>) {
        return makeExpectedParser(parser.parser(), what);
    }
    else {
        return ExpectedParser<Parser> {std::move(parser), what};
    }
}

} // namespace skarn::parser::details
