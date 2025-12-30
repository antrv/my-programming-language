#pragma once

#include "ParserContext.h"
#include <concepts>

namespace skarn::parser::details {

template <std::predicate<char> Predicate>
class CharPredicateParser final {
    Predicate predicate_;
    std::string_view what_;

public:
    using ParserType = CharPredicateParser;
    using ValueType = char;
    using InputType = char;

    explicit constexpr CharPredicateParser(Predicate predicate, const std::string_view what) noexcept
        : predicate_ {std::move(predicate)}
        , what_ {what} {
    }

    bool parse(ParserContext<char>& ctx, char& value) const {
        const std::span<const char> input = ctx.input();
        if (input.empty()) {
            if (what_.empty()) {
                ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0001, "Unexpected end of input");
            }
            else {
                ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0001, "Unexpected end of input, expected {}", what_);
            }

            return false;
        }

        const char chr = input[0];
        if (!predicate_(chr)) {
            if (what_.empty()) {
                ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0002, "Unexpected input '{}'", chr);
            }
            else {
                ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0002, "Unexpected input '{}', expected {}", chr, what_);
            }

            return false;
        }

        value = chr;
        ctx.consume(1);
        return true;
    }
};

} // namespace skarn::parser::details
