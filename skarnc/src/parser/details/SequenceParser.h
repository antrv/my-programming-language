#pragma once

#include "ParserContext.h"
#include <limits>

namespace skarn::parser::details {

template <Parser Parser, size_t MinCount = 0, size_t MaxCount = std::numeric_limits<size_t>::max()>
requires (MinCount < MaxCount)
class SequenceParser final {
    Parser parser_;

public:
    using ParserType = SequenceParser;
    using InputType = Parser::InputType;
    using ValueType = std::vector<typename Parser::ValueType>;

    explicit constexpr SequenceParser(Parser parser) noexcept
        : parser_ {std::move(parser)} {
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const {
        if constexpr (MinCount != 0) {
            for (size_t i = MinCount; i != 0; --i) {
                if (typename Parser::ValueType& val = value.emplace_back();
                    !parser_.parse(ctx, val)) {
                    value.pop_back();
                    return false;
                    }
            }
        }

        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        for (size_t i = MaxCount - MinCount; i != 0; --i) {
            const ParserPosition position = ctx.position();
            if (typename Parser::ValueType& val = value.emplace_back();
                !parser_.parse(ctx, val)) {
                value.pop_back();
                ctx.position(position);
                break;
            }
        }

        ctx.report_messages(report_flag);
        return true;
    }
};

} // namespace skarn::parser::details
