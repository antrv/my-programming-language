#pragma once

#include <expected>
#include "details/CombinedParser.h"
#include "details/ExpectedParser.h"
#include "details/OptionalParser.h"
#include "details/VariantParser.h"

namespace skarn::parser {

template <class Value>
using ParserResult = std::expected<Value, ParseMessages>;

template <details::Parser Parser>
class ParserInterface final {
    Parser parser_;

public:
    using ParserType = Parser;
    using ValueType = Parser::ValueType;

    constexpr ParserInterface() noexcept(std::is_nothrow_constructible_v<Parser>)
    requires (std::default_initializable<Parser>)
        : parser_ {} {
    }

    constexpr /*implicit*/ ParserInterface(Parser parser) noexcept(std::is_nothrow_move_constructible_v<Parser>)
        : parser_ {std::move(parser)} {
    }

    constexpr const Parser& parser() const noexcept {
        return parser_;
    }

    ParserResult<ValueType> parse(const std::string_view source) const {
        details::ParserContext<char> context {source};

        if (ValueType value {}; parser_.parse(context, value)) {
            return {std::move(value)};
        }

        return std::unexpected(std::move(context).messages());
    }

    template <details::Parser NextParser>
    constexpr auto operator >>(const ParserInterface<NextParser>& next) const noexcept {
        using ResultParser = decltype(details::makeCombinedParser(parser_, next.parser()));
        return ParserInterface<ResultParser> {details::makeCombinedParser(parser_, next.parser())};
    }

    template <details::Parser NextParser>
    constexpr auto operator ||(const ParserInterface<NextParser>& next) const noexcept {
        using ResultParser = decltype(details::makeVariantParser(parser_, next.parser()));
        return ParserInterface<ResultParser> {details::makeVariantParser(parser_, next.parser())};
    }

    constexpr auto expected(const std::string_view what) const noexcept {
        return ParserInterface<details::ExpectedParser<Parser>> {parser_, what};
    }

    constexpr auto optional() const noexcept {
        return ParserInterface<details::OptionalParser<Parser>> {};
    }
};

} // namespace skarn::parser
