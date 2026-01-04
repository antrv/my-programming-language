#pragma once

#include "ParserContext.h"
#include <tuple>
#include <variant>

namespace skarn::parser::details {

template <Parser...Parsers>
requires (sizeof...(Parsers) >= 2 && CompatibleParsers<Parsers...>)
class VariantParser final {
public:
    using ParserType = VariantParser;
    using ValueType = std::variant<typename Parsers::ValueType...>;
    using InputType = InputTypeOf<Parsers...>;

private:
    std::tuple<Parsers...> parsers_;

    [[nodiscard]] constexpr const std::tuple<Parsers...>& parsers() const noexcept {
        return parsers_;
    }

    bool parseLastVariant(ParserContext<InputType>& ctx, ValueType& value) const {
        constexpr size_t lastIndex = sizeof...(Parsers) - 1;
        auto& val = value.template emplace<lastIndex>();
        const auto& parser = std::get<lastIndex>(parsers_);
        return parser.parse(ctx, val);
    }

    bool parseLastVariant(ParserContext<InputType>& ctx) const {
        constexpr size_t lastIndex = sizeof...(Parsers) - 1;
        const auto& parser = std::get<lastIndex>(parsers_);
        return parser.parse(ctx);
    }

    template <size_t Index>
    bool parseVariant(ParserContext<InputType>& ctx, ValueType& value) const {
        const ParserPosition position = ctx.position();

        auto& val = value.template emplace<Index>();
        if (const auto& parser = std::get<Index>(parsers_);
            !parser.parse(ctx, val)) {
            ctx.position(position);
            return false;
        }

        return true;
    }

    template <size_t Index>
    bool parseVariant(ParserContext<InputType>& ctx) const {
        const ParserPosition position = ctx.position();

        if (const auto& parser = std::get<Index>(parsers_);
            !parser.parse(ctx)) {
            ctx.position(position);
            return false;
        }

        return true;
    }

    template <size_t...Indices>
    bool parseImpl(ParserContext<InputType>& ctx, ValueType& value, std::index_sequence<Indices...>) const {
        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        const bool result = (parseVariant<Indices>(ctx, value) || ...);
        ctx.report_messages(report_flag);

        return result || parseLastVariant(ctx, value);
    }

    template <size_t...Indices>
    bool parseImpl(ParserContext<InputType>& ctx, std::index_sequence<Indices...>) const {
        const bool report_flag = ctx.report_messages();
        ctx.report_messages(false);

        const bool result = (parseVariant<Indices>(ctx) || ...);
        ctx.report_messages(report_flag);

        return result || parseLastVariant(ctx);
    }

public:
    explicit constexpr VariantParser(Parsers... parsers) noexcept
        : parsers_ {std::move(parsers)...} {
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const {
        return parseImpl(ctx, value, std::make_index_sequence<sizeof...(Parsers) - 1>());
    }

    bool parse(ParserContext<InputType>& ctx) const {
        return parseImpl(ctx, std::make_index_sequence<sizeof...(Parsers) - 1>());
    }
};

template <Parser Parser1, Parser Parser2, Parser...Parsers>
requires (CompatibleParsers<Parser1, Parser2, Parsers...>)
constexpr auto makeVariantParser(Parser1 parser1, Parser2 parser2, Parsers... parsers) noexcept {
    // Helper to turn a single parser into a tuple (for concatenation)
    const auto toTuple = []<Parser P>(P&& p) static constexpr noexcept {
        if constexpr (SpecializationOf<P, VariantParser>) {
            return p.parsers(); // It's a VariantParser, return its internal tuple
        } else {
            return std::tuple<std::decay_t<P>> {std::forward<P>(p)};
        }
    };

    return std::apply([]<Parser...FlattenedParsers>(FlattenedParsers...flattenedParsers) static constexpr noexcept {
        return VariantParser<std::decay_t<FlattenedParsers>...> {std::move(flattenedParsers)...};
    }, std::tuple_cat(toTuple(std::move(parser1)), toTuple(std::move(parser2)), toTuple(std::move(parsers))...));
}

} // namespace skarn::parser::details
