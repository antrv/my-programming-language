#pragma once

#include "ParserContext.h"

namespace skarn::parser {

class LiteralParser final {
    std::string_view literal_;

public:
    using ParserType = LiteralParser;
    using InputType = char;
    using ValueType = std::string_view;

    explicit constexpr LiteralParser(const std::string_view literal) noexcept
        : literal_ {literal}
    {
    }

    bool parse(ParserContext<char>& ctx, std::string_view& value) const {
        const std::span<const char> input = ctx.input();
        if (input.empty()) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0001, "'{}'", literal_);
            return false;
        }

        const std::string_view str {input.data(), input.size()};
        if (!str.starts_with(literal_)) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0002, "'{}'", literal_);
            return false;
        }

        value = str.substr(0, literal_.length());
        ctx.consume(literal_.length());
        return true;
    }

    bool parse(ParserContext<char>& ctx) const {
        std::string_view value;
        return parse(ctx, value);
    }
};

} // namespace skarn::parser
