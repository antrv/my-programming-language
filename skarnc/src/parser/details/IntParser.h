#pragma once

#include "ParserContext.h"
#include <charconv>

namespace skarn::parser::details {

template <std::integral T, int Base = 10>
class IntParser final {
public:
    using ParserType = IntParser;
    using InputType = char;
    using ValueType = T;

    bool parse(ParserContext<char>& ctx, T& value) const {
        const std::span<const char> input = ctx.input();
        if (input.empty()) {
            ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0001, "Unexpected end of input, expected an integer");
            return false;
        }

        const auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value, Base);
        if (ec == std::errc::invalid_argument) {
            ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0002, "Unexpected input, expected an integer");
            return false;
        }

        if (ec == std::errc::result_out_of_range) {
            ctx.addMsg(ParserMsgLevel::Error, ParserMsgCode::C0003, "An integer is out of range");
            return false;
        }

        ctx.consume(static_cast<std::size_t>(std::distance(input.data(), ptr)));
        return true;
    }
};

} // namespace skarn::parser::details
