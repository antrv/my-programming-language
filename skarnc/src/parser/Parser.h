#pragma once

#include <expected>
#include "details/CharParser.h"
#include "details/CharPredicateParser.h"
#include "details/ElemParser.h"
#include "details/IntParser.h"
#include "details/LiteralParser.h"
#include "details/ReferenceParser.h"
#include "details/ValueParser.h"
#include "ParserInterface.h"

namespace skarn::parser {

struct Parse final {
    Parse() = delete;

    template <class Elem>
    static constexpr ParserInterface<details::ValueParser<std::decay_t<Elem>>> value(Elem&& val) noexcept {
        return details::ValueParser<std::decay_t<Elem>> {std::forward<Elem>(val)};
    }

    template <class Elem>
    static constexpr ParserInterface<details::ElemParser<std::decay_t<Elem>>> elem(Elem&& val) noexcept {
        return details::ElemParser<std::decay_t<Elem>> {std::forward<Elem>(val)};
    }

    static constexpr ParserInterface<details::CharParser> char_(const char character) noexcept {
        return details::CharParser {character};
    }

    template <std::predicate<char> Predicate>
    static constexpr ParserInterface<details::CharPredicateParser<Predicate>> char_(Predicate predicate,
        const std::string_view what = {}) noexcept {
        return details::CharPredicateParser {std::move(predicate), what};
    }

    static constexpr auto ws(const std::string_view what = {}) noexcept {
        using namespace std::string_view_literals;
        return char_([](const char chr) static noexcept {
            switch (chr) {
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    return true;
                default:
                    return false;
            };
        }, what.empty() ? "whitespace"sv : what);
    }

    static constexpr ParserInterface<details::LiteralParser> literal(const std::string_view str) noexcept {
        return details::LiteralParser {str};
    }

    template <std::integral T = int>
    static constexpr ParserInterface<details::IntParser<T>> integer() noexcept {
        return {};
    }

    template <class Value>
    static constexpr ParserInterface<details::ReferenceParser<Value>> ref() noexcept {
        return {};
    }
};

} // namespace skarn::parser
