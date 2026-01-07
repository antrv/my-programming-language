#pragma once

#include "ParserContext.h"
#include <limits>

namespace skarn::parser::details {

template <Parser Parser, size_t RequiredCount = 0, size_t OptionalMaxCount = std::numeric_limits<size_t>::max()>
class SequenceParser final {
    Parser parser_;

public:
    using ParserType = SequenceParser;
    using InputType = Parser::InputType;
    using ValueType =
        std::conditional_t<std::is_same_v<typename Parser::ValueType, NoValueType>, NoValueType,
            std::conditional_t<std::is_same_v<typename Parser::ValueType, char>, std::string,
            std::vector<typename Parser::ValueType>>>;

    explicit constexpr SequenceParser(Parser parser) noexcept
        : parser_ {std::move(parser)} {
    }

    bool parse(ParserContext<InputType>& ctx) const {
        if constexpr (RequiredCount != 0) {
            for (size_t i = RequiredCount; i != 0; --i) {
                if (!parser_.parse(ctx)) {
                    return false;
                }
            }
        }

        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        for (size_t i = OptionalMaxCount; i != 0; --i) {
            const ParserPosition position = ctx.position();
            if (!parser_.parse(ctx)) {
                ctx.position(position);
                break;
            }
        }

        ctx.report_messages(report_flag);
        return true;
    }

    // string
    bool parse(ParserContext<InputType>& ctx, std::string& value) const
    requires (std::is_same_v<typename Parser::ValueType, char>) {
        if constexpr (RequiredCount != 0) {
            for (size_t i = RequiredCount; i != 0; --i) {
                char val {};
                if (!parser_.parse(ctx, val)) {
                    return false;
                }

                value += val;
            }
        }

        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        for (size_t i = OptionalMaxCount; i != 0; --i) {
            const ParserPosition position = ctx.position();
            char val {};
            if (!parser_.parse(ctx, val)) {
                ctx.position(position);
                break;
            }

            value += val;
        }

        ctx.report_messages(report_flag);
        return true;
    }

    // vector
    bool parse(ParserContext<InputType>& ctx, ValueType& value) const
    requires (!OneOf<typename Parser::ValueType, NoValueType, char>) {
        if constexpr (RequiredCount != 0) {
            for (size_t i = RequiredCount; i != 0; --i) {
                if (typename Parser::ValueType& val = value.emplace_back();
                    !parser_.parse(ctx, val)) {
                    value.pop_back();
                    return false;
                }
            }
        }

        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        for (size_t i = OptionalMaxCount; i != 0; --i) {
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
