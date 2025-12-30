#pragma once

#include "ParserContext.h"

namespace skarn::parser::details {

class LiteralParser final {
    std::string_view literal_;

public:
    using ParserType = LiteralParser;
    using ValueType = std::string_view;
    using InputType = char;

    explicit constexpr LiteralParser(const std::string_view literal) noexcept
        : literal_ {literal}
    {
    }

    bool parse(ParserContext<char>& ctx, std::string_view& value) const {
        const std::span<const char> input = ctx.input();
        if (input.empty()) {
            ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0001, "Unexpected end of input, expected '{}'", literal_);
            return false;
        }

        const std::string_view str {input.data(), input.size()};
        if (!str.starts_with(literal_)) {
            ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0002, "Unexpected input '{}', expected '{}'", str, literal_);
            return false;
        }

        value = str.substr(0, literal_.length());
        ctx.consume(literal_.length());
        return true;
    }
};

} // namespace skarn::parser::details
