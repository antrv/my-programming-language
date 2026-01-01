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

    template <size_t Index>
    bool parseIndexImpl(ParserContext<InputType>& ctx, ValueType& value) const {
        auto& val = value.template emplace<Index>();
        const auto& parser = std::get<Index>(parsers_);
        return parser.parse(ctx, val);
    }

    template <size_t...Indices>
    bool parseImpl(ParserContext<InputType>& ctx, ValueType& value, std::index_sequence<Indices...>) const {
        return (parseIndexImpl<Indices>(ctx, value) || ...);
    }

public:
    explicit constexpr VariantParser(Parsers... parsers) noexcept
        : parsers_ {std::move(parsers)...} {
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const {
        return parseImpl(ctx, value, std::make_index_sequence<sizeof...(Parsers)>());
    }
};

template <Parser...Parsers>
requires (sizeof...(Parsers) >= 2 && CompatibleParsers<Parsers...>)
constexpr auto makeVariantParser(Parsers... parsers) noexcept {
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
    }, std::tuple_cat(toTuple(std::move(parsers))...));
}

} // namespace skarn::parser::details
