#pragma once

#include "ParserContext.h"
#include <optional>

namespace skarn::parser::details {

template <Parser Parser>
class OptionalParser;

template <Parser Parser>
requires (!std::is_same_v<typename Parser::ValueType, NoValueType>)
class OptionalParser<Parser> final {
    Parser parser_;

public:
    using ParserType = OptionalParser;
    using InputType = Parser::InputType;
    using ValueType = std::optional<typename Parser::ValueType>;

    explicit constexpr OptionalParser(Parser parser) noexcept
        : parser_ {std::move(parser)} {
    }

    [[nodiscard]] constexpr const Parser& parser() const noexcept {
        return parser_;
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const {
        value.emplace();
        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);
        const ParserPosition position = ctx.position();
        if (!parser_.parse(ctx, *value)) {
            value.reset();
            ctx.position(position); // restore position
        }

        ctx.report_messages(report_flag); // restore report flag
        return true;
    }

    bool parse(ParserContext<InputType>& ctx) const {
        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);
        const ParserPosition position = ctx.position();
        if (!parser_.parse(ctx)) {
            ctx.position(position); // restore position
        }

        ctx.report_messages(report_flag); // restore report flag
        return true;
    }
};

template <Parser Parser>
requires (std::is_same_v<typename Parser::ValueType, NoValueType>)
class OptionalParser<Parser> final {
    Parser parser_;

public:
    using ParserType = OptionalParser;
    using InputType = Parser::InputType;
    using ValueType = NoValueType; // TODO: maybe use bool? do consumers want to know when the underlying parser succeeds?

    explicit constexpr OptionalParser(Parser parser) noexcept
        : parser_ {std::move(parser)} {
    }

    [[nodiscard]] constexpr const Parser& parser() const noexcept {
        return parser_;
    }

    bool parse(ParserContext<InputType>& ctx) const {
        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);
        const ParserPosition position = ctx.position();
        if (!parser_.parse(ctx)) {
            ctx.position(position); // restore position
        }

        ctx.report_messages(report_flag); // restore report flag
        return true;
    }

    bool parse(ParserContext<InputType>& ctx, [[maybe_unused]] ValueType& value) const {
        return parse(ctx);
    }
};

template <Parser Parser>
OptionalParser(Parser) -> OptionalParser<std::decay_t<Parser>>;

template <Parser Parser>
constexpr auto makeOptionalParser(Parser parser) noexcept {
    using ParserType = std::decay_t<Parser>;
    if constexpr (SpecializationOf<ParserType, OptionalParser>) {
        return parser;
    }
    else {
        return OptionalParser<ParserType> {std::move(parser)};
    }
}

} // namespace skarn::parser::details
