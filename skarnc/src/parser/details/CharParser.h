#pragma once

#include "ParserContext.h"

namespace skarn::parser::details {

class CharParser final {
    char char_;

public:
    using ParserType = CharParser;
    using InputType = char;
    using ValueType = char;

    explicit constexpr CharParser(const char character) noexcept
        : char_ {character} {
    }

    bool parse(ParserContext<char>& ctx, char& value) const {
        const std::span<const char> input = ctx.input();
        if (input.empty()) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0001, "'{}'", char_);
            return false;
        }

        if (const char chr = input[0]; chr != char_) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0002, "'{}'", char_);
            return false;
        }

        value = char_;
        ctx.consume(1);
        return true;
    }
};

} // namespace skarn::parser::details
