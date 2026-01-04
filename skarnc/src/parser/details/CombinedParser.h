#pragma once

#include "ParserContext.h"
#include "TypePack.h"
#include <tuple>

namespace skarn::parser::details {

template <Parser...Parsers>
requires (sizeof...(Parsers) >= 2 && CompatibleParsers<Parsers...>)
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
    using InputType = InputTypeOf<Parsers...>;
    using ValueTypePack = TypePack<typename Parsers::ValueType...>::template remove_t<NoValueType>;
    using ValueType = std::conditional_t<ValueTypePack::size >= 2,
        typename ValueTypePack::template apply_to_t<std::tuple>,
        typename ValueTypePack::template first_or_default_t<NoValueType>>;

    explicit constexpr CombinedParser(Parsers... parsers) noexcept
        : parsers_ {std::move(parsers)...} {
    }

    [[nodiscard]] constexpr const std::tuple<Parsers...>& parsers() const noexcept {
        return parsers_;
    }

    // for tuple
    bool parse(ParserContext<InputType>& ctx, ValueType& value) const
    requires (ValueTypePack::size > 1) {
        return [&ctx, &value, &parsers = parsers_]<size_t...Indices>(const std::index_sequence<Indices...>) {
            return ((std::get<Indices>(parsers).parse(ctx, std::get<value_index<Indices>>(value))) && ...);
        }(std::make_index_sequence<sizeof...(Parsers)>());
    }

    bool parse(ParserContext<InputType>& ctx) const {
        return [&ctx, &parsers = parsers_]<size_t...Indices>(const std::index_sequence<Indices...>) {
            return ((std::get<Indices>(parsers).parse(ctx)) && ...);
        }(std::make_index_sequence<sizeof...(Parsers)>());
    }

    // for NoValueType
    bool parse(ParserContext<InputType>& ctx, [[maybe_unused]] ValueType& value) const
    requires (ValueTypePack::size == 0) {
        return parse(ctx);
    }

    // for a single value
    bool parse(ParserContext<InputType>& ctx, ValueType& value) const
    requires (ValueTypePack::size == 1) {
        return [&ctx, &value, &parsers = parsers_]<size_t...Indices>(const std::index_sequence<Indices...>) {
            using SingleValueType = ValueTypePack::template element_t<0>;
            constexpr size_t indexOfSingleValue = TypePack<typename Parsers::ValueType...>::template index_of<SingleValueType>;
            return ((indexOfSingleValue == Indices ? std::get<Indices>(parsers).parse(ctx, value) : std::get<Indices>(parsers).parse(ctx)) && ...);
        }(std::make_index_sequence<sizeof...(Parsers)>());
    }
};

template <Parser...Parsers>
requires (sizeof...(Parsers) >= 2 && CompatibleParsers<Parsers...>)
constexpr auto makeCombinedParser(Parsers... parsers) noexcept {
    // Helper to turn a single parser into a tuple (for concatenation)
    const auto toTuple = []<Parser P>(P&& p) static constexpr noexcept {
        if constexpr (SpecializationOf<P, CombinedParser>) {
            return p.parsers(); // It's a CombinedParser, return its internal tuple
        } else {
            return std::tuple<std::decay_t<P>> {std::forward<P>(p)};
        }
    };

    return std::apply([]<Parser...FlattenedParsers>(FlattenedParsers...flattenedParsers) static constexpr noexcept {
        return CombinedParser<std::decay_t<FlattenedParsers>...> {std::move(flattenedParsers)...};
    }, std::tuple_cat(toTuple(std::move(parsers))...));
}

} // namespace skarn::parser::details
