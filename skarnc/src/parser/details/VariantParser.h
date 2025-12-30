#pragma once

#include "ParserContext.h"
#include "Concepts.h"
#include <tuple>
#include <variant>

namespace skarn::parser::details {

template <class Elem, IsParser<Elem>...Parsers>
requires (sizeof...(Parsers) >= 2)
class VariantParser final {
    std::tuple<Parsers...> parsers_;

    template <size_t Index>
    bool parseIndexImpl(ParserContext<Elem>& ctx, std::variant<typename Parsers::ValueType...>& value) {
        auto& val = value.template emplace<Index>();
        const auto& parser = std::get<Index>(parsers_);
        return parser.parse(ctx, val);
    }

    template <size_t...Indices>
    bool parseImpl(ParserContext<Elem>& ctx, std::variant<typename Parsers::ValueType...>& value,
        std::index_sequence<Indices...>) const {
        return (parseIndexImpl<Indices>(ctx, value) || ...);
    }

public:
    using ParserType = VariantParser;
    using ValueType = std::variant<typename Parsers::ValueType...>;
    using InputType = Parsers::ValueType...;

    explicit constexpr VariantParser(Parsers... parsers) noexcept
        : parsers_ {std::move(parsers)...} {
    }

    bool parse(ParserContext<Elem>& ctx, ValueType& value) const {
        return parseImpl(ctx, value, std::make_index_sequence<sizeof...(Parsers)>());
    }
};

template <class Elem, IsParser<Elem>...Parsers>
requires (sizeof...(Parsers) >= 2)
constexpr auto makeVariantParser(Parsers... parsers) noexcept {
    // Helper to turn a single parser into a tuple (for concatenation)
    const auto toTuple = []<IsParser<Elem> P>(P&& p) static constexpr noexcept {
        if constexpr (IsSpecializationOf<P, VariantParser>) {
            return p.parsers(); // It's a VariantParser, return its internal tuple
        } else {
            return std::tuple<std::decay_t<P>> {std::forward<P>(p)};
        }
    };

    return std::apply([]<IsParser<Elem>...FlattenedParsers>(FlattenedParsers...flattenedParsers) static constexpr noexcept {
        return VariantParser<Elem, std::decay_t<FlattenedParsers>...>{std::move(flattenedParsers)...};
    }, std::tuple_cat(toTuple(std::move(parsers))...));
}

} // namespace skarn::parser::details
