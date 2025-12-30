#pragma once

#include "ParserContext.h"
#include "Concepts.h"
#include <tuple>

namespace skarn::parser::details {

template <IsParser...Parsers>
requires (sizeof...(Parsers) >= 2 && CompatibleParsers<Parsers...>)
class CombinedParser final {
    std::tuple<Parsers...> parsers_;

public:
    using ParserType = CombinedParser;
    using ValueType = std::tuple<typename Parsers::ValueType...>;
    using InputType = typename Parsers::InputType;

    explicit constexpr CombinedParser(Parsers... parsers) noexcept
        : parsers_ {std::move(parsers)...} {
    }

    [[nodiscard]] constexpr const std::tuple<Parsers...>& parsers() const noexcept {
        return parsers_;
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const {
        return [&ctx, &value, &parsers = parsers_]<size_t...Indices>(const std::index_sequence<Indices...>) {
            return ((std::get<Indices>(parsers).parse(ctx, std::get<Indices>(value))) && ...);
        }(std::make_index_sequence<sizeof...(Parsers)>());
    }
};

template <IsParser...Parsers>
requires (sizeof...(Parsers) >= 2 && CompatibleParsers<Parsers...>)
constexpr auto makeCombinedParser(Parsers... parsers) noexcept {
    // Helper to turn a single parser into a tuple (for concatenation)
    const auto toTuple = []<IsParser P>(P&& p) static constexpr noexcept {
        if constexpr (isSpecializationOf<P, CombinedParser>) {
            return p.parsers(); // It's a CombinedParser, return its internal tuple
        } else {
            return std::tuple<std::decay_t<P>> {std::forward<P>(p)};
        }
    };

    return std::apply([]<IsParser...FlattenedParsers>(FlattenedParsers...flattenedParsers) static constexpr noexcept {
        return CombinedParser<std::decay_t<FlattenedParsers>...>{std::move(flattenedParsers)...};
    }, std::tuple_cat(toTuple(std::move(parsers))...));
}

} // namespace skarn::parser::details
