#pragma once

#include <expected>
#include "details/CharParser.h"
#include "details/CombinedParser.h"
#include "details/ExpectedParser.h"
#include "details/IgnoreParser.h"
#include "details/LiteralParser.h"
#include "details/OptionalParser.h"
#include "details/ReferenceParser.h"
#include "details/SequenceParser.h"
#include "details/TransformParser.h"
#include "details/VariantParser.h"

namespace skarn::parser {

template <class Value>
using ParserResult = std::expected<Value, ParseMessages>;

template <details::Parser Parser>
class ParserInterface final {
    Parser parser_;

public:
    using ParserType = Parser;
    using InputType = Parser::InputType;
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

    constexpr auto operator >>(const char chr) const noexcept {
        using ResultParser = decltype(details::makeCombinedParser(parser_, details::CharParser {chr}));
        return ParserInterface<ResultParser> {details::makeCombinedParser(parser_, details::CharParser {chr})};
    }

    constexpr auto operator >>(const std::string_view literal) const noexcept {
        using ResultParser = decltype(details::makeCombinedParser(parser_, details::LiteralParser {literal}));
        return ParserInterface<ResultParser> {details::makeCombinedParser(parser_, details::LiteralParser {literal})};
    }

    template <details::TransformInvocable<typename Parser::ValueType> Invocable>
    constexpr auto operator >>(Invocable invocable) const noexcept {
        return ParserInterface<details::TransformParser<Parser, Invocable>> {
            details::TransformParser<Parser, Invocable> {parser_, std::move(invocable)}};
    }

    template <details::Parser NextParser>
    constexpr auto operator ||(const ParserInterface<NextParser>& next) const noexcept {
        using ResultParser = decltype(details::makeVariantParser(parser_, next.parser()));
        return ParserInterface<ResultParser> {details::makeVariantParser(parser_, next.parser())};
    }

    constexpr ParserInterface<details::SequenceParser<Parser>> operator *() const noexcept {
        return ParserInterface<details::SequenceParser<Parser>> {details::SequenceParser<Parser> {parser_}};
    }

    constexpr ParserInterface<details::SequenceParser<Parser, 1>> operator +() const noexcept {
        return ParserInterface<details::SequenceParser<Parser, 1>> {details::SequenceParser<Parser, 1> {parser_}};
    }

    constexpr ParserInterface<details::IgnoreParser<Parser>> operator ~() const noexcept {
        using ResultParser = decltype(details::makeIgnoreParser(parser_));
        return ParserInterface<ResultParser> {details::makeIgnoreParser(parser_)};
    }

    constexpr auto expected(const std::string_view what) const noexcept {
        using ResultParser = decltype(details::makeExpectedParser(parser_, what));
        return ParserInterface<ResultParser> {details::makeExpectedParser(parser_, what)};
    }

    constexpr auto optional() const noexcept {
        using ResultParser = decltype(details::makeOptionalParser(parser_));
        return ParserInterface<ResultParser> {details::makeOptionalParser(parser_)};
    }

    template <details::Parser WrapParser>
    constexpr auto wrap(const ParserInterface<WrapParser>& wrapParser) const noexcept {
        return ~wrapParser >> *this >> ~wrapParser;
    }

    template <details::Parser SeparatorParser>
    constexpr auto seq(const ParserInterface<SeparatorParser>& separatorParser) const noexcept {
        return
            *this >> *(~separatorParser >> *this) >>
            [](std::vector<ValueType>& result, std::tuple<ValueType, std::vector<ValueType>>& value) static {
                result.push_back(std::move(std::get<0>(value))); // TODO: extend the SequenceParser to accept a separator
                result.append_range(std::move(std::get<1>(value)));
            };
    }

    constexpr auto seq(const char chr) const noexcept {
        return seq(ParserInterface {details::CharParser {chr}});
    }

    template <details::Parser ParserRef>
    requires (
        SpecializationOf<Parser, details::ReferenceParser> &&
        std::is_same_v<ValueType, typename ParserRef::ValueType> &&
        std::is_same_v<InputType, typename ParserRef::InputType>)
    void assign(ParserRef parser) const {
        parser_.assign(std::move(parser));
    }
};

template <details::Parser Parser>
constexpr auto operator >>(const char chr, const ParserInterface<Parser>& parser) noexcept {
    using ResultParser = decltype(details::makeCombinedParser(details::CharParser {chr}, parser.parser()));
    return ParserInterface<ResultParser> {details::makeCombinedParser(details::CharParser {chr}, parser.parser())};
}

template <details::Parser Parser>
constexpr auto operator >>(const std::string_view literal, const ParserInterface<Parser>& parser) noexcept {
    using ResultParser = decltype(details::makeCombinedParser(details::LiteralParser {literal}, parser.parser()));
    return ParserInterface<ResultParser> {details::makeCombinedParser(details::LiteralParser {literal}, parser.parser())};
}

} // namespace skarn::parser
