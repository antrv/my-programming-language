#pragma once

#include "ParserContext.h"
#include <concepts>

namespace skarn::parser {

template <std::predicate<char> Predicate>
class CharPredicateParser final {
    Predicate predicate_;
    std::string_view what_;

public:
    using ParserType = CharPredicateParser;
    using InputType = char;
    using ValueType = char;

    explicit constexpr CharPredicateParser(Predicate predicate, const std::string_view what) noexcept
        : predicate_ {std::move(predicate)}
        , what_ {what} {
    }

    bool parse(ParserContext<char>& ctx, char& value) const {
        const std::span<const char> input = ctx.input();
        if (input.empty()) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0001, "{}", what_);
            return false;
        }

        const char chr = input[0];
        if (!predicate_(chr)) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0002, "{}", what_);
            return false;
        }

        value = chr;
        ctx.consume(1);
        return true;
    }

    bool parse(ParserContext<char>& ctx) const {
        char value {};
        return parse(ctx, value);
    }
};

} // namespace skarn::parser
