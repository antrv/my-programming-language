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
    static constexpr ParserInterface<ValueParser<std::decay_t<Elem>>> value(Elem&& val) noexcept {
        return ValueParser<std::decay_t<Elem>> {std::forward<Elem>(val)};
    }

    template <class Elem>
    static constexpr ParserInterface<ElemParser<std::decay_t<Elem>>> elem(Elem&& val) noexcept {
        return ElemParser<std::decay_t<Elem>> {std::forward<Elem>(val)};
    }

    static constexpr ParserInterface<CharParser> char_(const char character) noexcept {
        return CharParser {character};
    }

    template <std::predicate<char> Predicate>
    static constexpr ParserInterface<CharPredicateParser<Predicate>> char_(Predicate predicate,
        const std::string_view what = {}) noexcept {
        return CharPredicateParser {std::move(predicate), what};
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

    static constexpr ParserInterface<LiteralParser> literal(const std::string_view str) noexcept {
        return LiteralParser {str};
    }

    template <std::integral T = int>
    static constexpr ParserInterface<IntParser<T>> integer() noexcept {
        return {};
    }

    template <class Value>
    static constexpr ParserInterface<ReferenceParser<Value>> ref() noexcept {
        return {};
    }
};

} // namespace skarn::parser
