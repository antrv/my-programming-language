#pragma once

#include "ParserContext.h"

namespace skarn::parser {

template <class Elem, class Eq = std::equal_to<Elem>>
class ElemParser final {
    Elem elem_;

public:
    using ParserType = ElemParser;
    using InputType = Elem;
    using ValueType = Elem;

    explicit constexpr ElemParser(Elem elem) noexcept
        : elem_ {std::move(elem)} {
    }

    bool parse(ParserContext<Elem>& ctx, Elem& value) const {
        const std::span<const Elem> input = ctx.input();
        if (input.empty()) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0001, "'{}'", elem_);
            return false;
        }

        const Elem& elem = input[0];
        if (!Eq {}(elem, elem_)) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0002, "'{}'", elem_);
            return false;
        }

        value = elem;
        ctx.consume(1);
        return true;
    }

    bool parse(ParserContext<Elem>& ctx) const {
        const std::span<const Elem> input = ctx.input();
        if (input.empty()) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0001, "'{}'", elem_);
            return false;
        }

        if (const Elem& elem = input[0];
            !Eq {}(elem, elem_)) {
            ctx.add_message(ParserMsgLevel::Error, ParserMsgCode::C0002, "'{}'", elem_);
            return false;
        }

        ctx.consume(1);
        return true;
    }
};

} // namespace skarn::parser
