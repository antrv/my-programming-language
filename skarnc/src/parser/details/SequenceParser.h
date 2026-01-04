#pragma once

#include "ParserContext.h"
#include <limits>

namespace skarn::parser::details {

template <Parser Parser, size_t MinCount = 0, size_t MaxCount = std::numeric_limits<size_t>::max()>
class SequenceParser;

template <Parser Parser, size_t MinCount, size_t MaxCount>
requires (MinCount < MaxCount && !OneOf<typename Parser::ValueType, char, NoValueType>)
class SequenceParser<Parser, MinCount, MaxCount> final {
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

    bool parse(ParserContext<InputType>& ctx) const {
        if constexpr (MinCount != 0) {
            for (size_t i = MinCount; i != 0; --i) {
                if (!parser_.parse(ctx)) {
                    return false;
                }
            }
        }

        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        for (size_t i = MaxCount - MinCount; i != 0; --i) {
            const ParserPosition position = ctx.position();
            if (!parser_.parse(ctx)) {
                ctx.position(position);
                break;
            }
        }

        ctx.report_messages(report_flag);
        return true;
    }
};

template <Parser Parser, size_t MinCount, size_t MaxCount>
requires (MinCount < MaxCount && std::is_same_v<typename Parser::ValueType, char>)
class SequenceParser<Parser, MinCount, MaxCount> final {
    Parser parser_;

public:
    using ParserType = SequenceParser;
    using InputType = Parser::InputType;
    using ValueType = std::string;

    explicit constexpr SequenceParser(Parser parser) noexcept
        : parser_ {std::move(parser)} {
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const {
        if constexpr (MinCount != 0) {
            for (size_t i = MinCount; i != 0; --i) {
                char val {};
                if (!parser_.parse(ctx, val)) {
                    return false;
                }

                value += val;
            }
        }

        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        for (size_t i = MaxCount - MinCount; i != 0; --i) {
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

    bool parse(ParserContext<InputType>& ctx) const {
        if constexpr (MinCount != 0) {
            for (size_t i = MinCount; i != 0; --i) {
                if (!parser_.parse(ctx)) {
                    return false;
                }
            }
        }

        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        for (size_t i = MaxCount - MinCount; i != 0; --i) {
            const ParserPosition position = ctx.position();
            if (!parser_.parse(ctx)) {
                ctx.position(position);
                break;
            }
        }

        ctx.report_messages(report_flag);
        return true;
    }
};

template <Parser Parser, size_t MinCount, size_t MaxCount>
requires (MinCount < MaxCount && std::is_same_v<typename Parser::ValueType, NoValueType>)
class SequenceParser<Parser, MinCount, MaxCount> final {
    Parser parser_;

public:
    using ParserType = SequenceParser;
    using InputType = Parser::InputType;
    using ValueType = NoValueType; // TODO: maybe use size_t? do consumers want to know how many times the underlying parser succeeds?

    explicit constexpr SequenceParser(Parser parser) noexcept
        : parser_ {std::move(parser)} {
    }

    bool parse(ParserContext<InputType>& ctx) const {
        if constexpr (MinCount != 0) {
            for (size_t i = MinCount; i != 0; --i) {
                if (!parser_.parse(ctx)) {
                    return false;
                }
            }
        }

        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        for (size_t i = MaxCount - MinCount; i != 0; --i) {
            const ParserPosition position = ctx.position();
            if (!parser_.parse(ctx)) {
                ctx.position(position);
                break;
            }
        }

        ctx.report_messages(report_flag);
        return true;
    }

    bool parse(ParserContext<InputType>& ctx, [[maybe_unused]] ValueType& value) const {
        return parse(ctx);
    }
};

template <Parser Parser>
SequenceParser(Parser parser) -> SequenceParser<std::decay_t<Parser>>;

} // namespace skarn::parser::details
