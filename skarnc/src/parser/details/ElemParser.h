#pragma once

#include "ParserContext.h"

namespace skarn::parser::details {

template <class Elem, class Eq = std::equal_to<Elem>>
class ElemParser final {
    Elem elem_;

public:
    using ParserType = ElemParser;
    using ValueType = Elem;
    using InputType = Elem;

    explicit constexpr ElemParser(Elem elem) noexcept
        : elem_ {std::move(elem)} {
    }

    bool parse(ParserContext<Elem>& ctx, Elem& value) const {
        const std::span<const Elem> input = ctx.input();
        if (input.empty()) {
            ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0001, "Unexpected end of input, expected '{}'", elem_);
            return false;
        }

        const Elem& elem = input[0];
        if (!Eq {}(elem, elem_)) {
            ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0002, "Unexpected input '{}', expected '{}'", elem, elem_);
            return false;
        }

        value = elem;
        ctx.consume(1);
        return true;
    }
};

} // namespace skarn::parser::details
