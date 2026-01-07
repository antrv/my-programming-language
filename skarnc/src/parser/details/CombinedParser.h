#pragma once

#include "ParserContext.h"
#include "TypePack.h"
#include <tuple>

namespace skarn::parser {

template <details::Parser...Parsers>
requires (sizeof...(Parsers) >= 2 && details::CompatibleParsers<Parsers...>)
class CombinedParser final {
    std::tuple<Parsers...> parsers_;

    // calculates index of a value in a tuple for a parser index
    template <size_t ParserIndex>
    static constexpr size_t value_index =
        TypePack<typename Parsers::ValueType...>::
            template first_n_t<ParserIndex>::
            template remove_t<NoValueType>::
            size;

public:
    using ParserType = CombinedParser;
    using InputType = details::InputTypeOf<Parsers...>;
    using ValueTypePack = TypePack<typename Parsers::ValueType...>::template remove_t<NoValueType>;
    using ValueType = std::conditional_t<ValueTypePack::size >= 2,
        typename ValueTypePack::template apply_to_t<std::tuple>,
        typename ValueTypePack::template first_or_default_t<NoValueType>>;

private:
    template <size_t Index>
    requires (ValueTypePack::size == 1)
    bool parseSingleValueItem(ParserContext<InputType>& ctx, ValueType& value) const {
        using SingleValueType = ValueTypePack::template element_t<0>;
        constexpr size_t indexOfSingleValue = TypePack<typename Parsers::ValueType...>::template index_of<SingleValueType>;
        const auto& parser = std::get<Index>(parsers_);
        if constexpr (Index == indexOfSingleValue) {
            return parser.parse(ctx, value);
        }
        else {
            return parser.parse(ctx);
        }
    }

    template <size_t...Indices>
    requires (ValueTypePack::size == 1)
    bool parseSingleValue(ParserContext<InputType>& ctx, ValueType& value, std::index_sequence<Indices...>) const {
        return (parseSingleValueItem<Indices>(ctx, value) && ...);
    }

    template <size_t Index>
    requires (ValueTypePack::size > 1)
    bool parseTupleItem(ParserContext<InputType>& ctx, ValueType& value) const {
        const auto& parser = std::get<Index>(parsers_);
        using ItemParserType = std::decay_t<decltype(parser)>;
        if constexpr (std::is_same_v<typename ItemParserType::ValueType, NoValueType>) {
            return parser.parse(ctx);
        }
        else {
            return parser.parse(ctx, std::get<value_index<Index>>(value));
        }
    }

    template <size_t...Indices>
    requires (ValueTypePack::size > 1)
    bool parseTuple(ParserContext<InputType>& ctx, ValueType& value, std::index_sequence<Indices...>) const {
        return (parseTupleItem<Indices>(ctx, value) && ...);
    }

public:

    explicit constexpr CombinedParser(Parsers... parsers) noexcept
        : parsers_ {std::move(parsers)...} {
    }

    [[nodiscard]] constexpr const std::tuple<Parsers...>& parsers() const noexcept {
        return parsers_;
    }

    // for tuple
    bool parse(ParserContext<InputType>& ctx, ValueType& value) const
    requires (ValueTypePack::size > 1) {
        return parseTuple(ctx, value, std::make_index_sequence<sizeof...(Parsers)>());
    }

    bool parse(ParserContext<InputType>& ctx) const {
        return [&ctx, &parsers = parsers_]<size_t...Indices>(const std::index_sequence<Indices...>) {
            return ((std::get<Indices>(parsers).parse(ctx)) && ...);
        }(std::make_index_sequence<sizeof...(Parsers)>());
    }

    // for a single value
    bool parse(ParserContext<InputType>& ctx, ValueType& value) const
    requires (ValueTypePack::size == 1) {
        return parseSingleValue(ctx, value, std::make_index_sequence<sizeof...(Parsers)>());
    }
};

template <details::Parser...Parsers>
requires (sizeof...(Parsers) >= 2 && details::CompatibleParsers<Parsers...>)
constexpr auto makeCombinedParser(Parsers... parsers) noexcept {
    // Helper to turn a single parser into a tuple (for concatenation)
    const auto toTuple = []<details::Parser P>(P&& p) static constexpr noexcept {
        if constexpr (SpecializationOf<P, CombinedParser>) {
            return p.parsers(); // It's a CombinedParser, return its internal tuple
        } else {
            return std::tuple<std::decay_t<P>> {std::forward<P>(p)};
        }
    };

    return std::apply([]<details::Parser...FlattenedParsers>(FlattenedParsers...flattenedParsers) static constexpr noexcept {
        return CombinedParser<std::decay_t<FlattenedParsers>...> {std::move(flattenedParsers)...};
    }, std::tuple_cat(toTuple(std::move(parsers))...));
}

} // namespace skarn::parser::details
