#pragma once

#include "ParserContext.h"
#include <tuple>
#include <variant>

namespace skarn::parser {

template <details::Parser...Parsers>
requires (sizeof...(Parsers) >= 2 && details::CompatibleParsers<Parsers...>)
class VariantParser final {
    std::tuple<Parsers...> parsers_;

    template <size_t Index>
    using ParserValueType = TypePack<typename Parsers::ValueType...>::template element_t<Index>;

public:
    using ParserType = VariantParser;
    using InputType = details::InputTypeOf<Parsers...>;
    using ValueTypePack = type_pack_unique_t<TypePack<typename Parsers::ValueType...>>;
    using ValueType = std::conditional_t<
        ValueTypePack::size == 1,
        typename ValueTypePack::template element_t<0>,
        typename ValueTypePack::template replace_t<NoValueType, std::monostate>::template apply_to_t<std::variant>>;

private:
    bool parseLastVariant(ParserContext<InputType>& ctx, ValueType& value) const {
        constexpr size_t lastIndex = sizeof...(Parsers) - 1;
        const auto& parser = std::get<lastIndex>(parsers_);
        using Value = ParserValueType<lastIndex>;
        if constexpr (std::is_same_v<ValueType, NoValueType>) {
            return parser.parse(ctx);
        }
        else if constexpr (!SpecializationOf<ValueType, std::variant>) {
            return parser.parse(ctx, value);
        }
        else if constexpr (std::is_same_v<Value, NoValueType>) {
            value.template emplace<std::monostate>();
            return parser.parse(ctx);
        }
        else {
            auto& val = value.template emplace<Value>();
            return parser.parse(ctx, val);
        }
    }

    bool parseLastVariant(ParserContext<InputType>& ctx) const {
        constexpr size_t lastIndex = sizeof...(Parsers) - 1;
        const auto& parser = std::get<lastIndex>(parsers_);
        return parser.parse(ctx);
    }

    template <size_t Index>
    bool parseVariant(ParserContext<InputType>& ctx, ValueType& value) const {
        const ParserPosition position = ctx.position();
        using Value = ParserValueType<Index>;
        const auto& parser = std::get<Index>(parsers_);
        if constexpr (std::is_same_v<ValueType, NoValueType>) {
            if (!parser.parse(ctx)) {
                ctx.position(position);
                return false;
            }
        }
        else if constexpr (!SpecializationOf<ValueType, std::variant>) {
            if (!parser.parse(ctx, value)) {
                ctx.position(position);
                return false;
            }
        }
        else if constexpr (std::is_same_v<Value, NoValueType>) {
            value.template emplace<std::monostate>();
            if (!parser.parse(ctx)) {
                ctx.position(position);
                return false;
            }
        }
        else {
            auto& val = value.template emplace<Value>();
            if (!parser.parse(ctx, val)) {
                ctx.position(position);
                return false;
            }
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

    [[nodiscard]] constexpr const std::tuple<Parsers...>& parsers() const noexcept {
        return parsers_;
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const
    requires (!std::is_same_v<ValueType, NoValueType>) {
        return parseImpl(ctx, value, std::make_index_sequence<sizeof...(Parsers) - 1>());
    }

    bool parse(ParserContext<InputType>& ctx) const {
        return parseImpl(ctx, std::make_index_sequence<sizeof...(Parsers) - 1>());
    }
};

template <details::Parser...Parsers>
requires (sizeof...(Parsers) >= 2 && details::CompatibleParsers<Parsers...>)
constexpr auto makeVariantParser(Parsers... parsers) noexcept {
    // Helper to turn a single parser into a tuple (for concatenation)
    const auto toTuple = []<details::Parser P>(P&& p) static constexpr noexcept {
        if constexpr (SpecializationOf<P, VariantParser>) {
            return p.parsers(); // It's a VariantParser, return its internal tuple
        } else {
            return std::tuple<std::decay_t<P>> {std::forward<P>(p)};
        }
    };

    return std::apply([]<details::Parser...FlattenedParsers>(FlattenedParsers...flattenedParsers) static constexpr noexcept {
        return VariantParser<std::decay_t<FlattenedParsers>...> {std::move(flattenedParsers)...};
    }, std::tuple_cat(toTuple(std::move(parsers))...));
}

} // namespace skarn::parser
