#pragma once

#include "ParserContext.h"
#include <optional>

namespace skarn::parser::details {

template <Parser Parser>
class OptionalParser final {
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
};

} // namespace skarn::parser::details
