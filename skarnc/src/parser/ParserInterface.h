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
#include "details/ValueParser.h"
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
        ParserContext<char> context {source};

        if (ValueType value {}; parser_.parse(context, value)) {
            return {std::move(value)};
        }

        return std::unexpected(std::move(context).messages());
    }

    template <details::Parser NextParser>
    constexpr auto operator >>(const ParserInterface<NextParser>& next) const noexcept {
        using ResultParser = decltype(makeCombinedParser(parser_, next.parser()));
        return ParserInterface<ResultParser> {makeCombinedParser(parser_, next.parser())};
    }

    constexpr auto operator >>(const char chr) const noexcept {
        using ResultParser = decltype(makeCombinedParser(parser_, CharParser {chr}));
        return ParserInterface<ResultParser> {makeCombinedParser(parser_, CharParser {chr})};
    }

    constexpr auto operator >>(const std::string_view literal) const noexcept {
        using ResultParser = decltype(makeCombinedParser(parser_, LiteralParser {literal}));
        return ParserInterface<ResultParser> {makeCombinedParser(parser_, LiteralParser {literal})};
    }

    template <details::TransformInvocable<typename Parser::ValueType> Invocable>
    constexpr auto operator >>(Invocable invocable) const noexcept {
        return ParserInterface<TransformParser<Parser, Invocable>> {
            TransformParser<Parser, Invocable> {parser_, std::move(invocable)}};
    }

    template <details::Parser NextParser>
    constexpr auto operator ||(const ParserInterface<NextParser>& next) const noexcept {
        using ResultParser = decltype(makeVariantParser(parser_, next.parser()));
        return ParserInterface<ResultParser> {makeVariantParser(parser_, next.parser())};
    }

    constexpr ParserInterface<SequenceParser<Parser>> operator *() const noexcept {
        return ParserInterface<SequenceParser<Parser>> {SequenceParser<Parser> {parser_}};
    }

    constexpr ParserInterface<SequenceParser<Parser, 1>> operator +() const noexcept {
        return ParserInterface<SequenceParser<Parser, 1>> {SequenceParser<Parser, 1> {parser_}};
    }

    constexpr ParserInterface<IgnoreParser<Parser>> operator ~() const noexcept {
        using ResultParser = decltype(makeIgnoreParser(parser_));
        return ParserInterface<ResultParser> {makeIgnoreParser(parser_)};
    }

    constexpr auto expected(const std::string_view what) const noexcept {
        using ResultParser = decltype(makeExpectedParser(parser_, what));
        return ParserInterface<ResultParser> {makeExpectedParser(parser_, what)};
    }

    constexpr auto optional() const noexcept {
        using ResultParser = decltype(makeOptionalParser(parser_));
        return ParserInterface<ResultParser> {makeOptionalParser(parser_)};
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
        return seq(ParserInterface {CharParser {chr}});
    }

    template <class Value>
    constexpr auto value(Value val) const noexcept {
        return ParserInterface<ValueParser<Parser, Value>> {ValueParser<Parser, Value> {parser_, val}};
    }

    template <details::Parser ParserRef>
    requires (
        SpecializationOf<Parser, ReferenceParser> &&
        std::is_same_v<ValueType, typename ParserRef::ValueType> &&
        std::is_same_v<InputType, typename ParserRef::InputType>)
    void assign(const ParserInterface<ParserRef>& parser) const {
        parser_.assign(parser.parser());
    }
};

template <details::Parser Parser>
constexpr auto operator >>(const char chr, const ParserInterface<Parser>& parser) noexcept {
    using ResultParser = decltype(makeCombinedParser(CharParser {chr}, parser.parser()));
    return ParserInterface<ResultParser> {makeCombinedParser(CharParser {chr}, parser.parser())};
}

template <details::Parser Parser>
constexpr auto operator >>(const std::string_view literal, const ParserInterface<Parser>& parser) noexcept {
    using ResultParser = decltype(makeCombinedParser(LiteralParser {literal}, parser.parser()));
    return ParserInterface<ResultParser> {makeCombinedParser(LiteralParser {literal}, parser.parser())};
}

} // namespace skarn::parser
