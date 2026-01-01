#pragma once

#include "ParserContext.h"
#include "TypePack.h"
#include <tuple>

namespace skarn::parser::details {

template <Parser...Parsers>
requires (sizeof...(Parsers) >= 2 && CompatibleParsers<Parsers...>)
class CombinedParser final {
    std::tuple<Parsers...> parsers_;

public:
    using ParserType = CombinedParser;
    using ValueType = std::tuple<typename Parsers::ValueType...>;
    using InputType = InputTypeOf<Parsers...>;

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
