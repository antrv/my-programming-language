#pragma once

#include <boost/parser/parser.hpp>
#include "Lexer.h"

namespace skarn::lexer {

using namespace boost::parser;
using namespace boost::parser::literals;

namespace {
// Rules
// bp::rule<struct UnitParser, ast::Unit> unitParser {"Unit"};
// bp::rule<struct FunctionParser, ast::Function> functionParser {"Function"};
// bp::rule<struct StatementParser, std::unique_ptr<ast::Statement>> statementParser {"Statement"};
// bp::rule<struct ExpressionParser, std::unique_ptr<ast::Expression>> expressionParser {"Expression"};

// Definitions
constexpr auto keywordParser =
    "var"_l | "let"_l | "fn"_l | "const"_l | "if"_l | "else"_l | "ret"_l | "while"_l | "true"_l | "false"_l | "for"_l;

constexpr auto identParser =
    ((char_('a', 'z') | char_('A', 'Z')) >>
    *(char_('a', 'z') | char_('A', 'Z') | char_('0', '9') | char_('_'))) - keywordParser;

const auto functionParser =
    "fn"_l >> *ws;

// const auto statementParser_def = bp::double_ % ',';
// const auto expressionParser_def = bp::double_ % ',';
// const auto functionParser_def = bp::lit("fn") >> *bp::ws >> bp::ident;
//
// const auto unitParser_def = *bp::ws >> *functionParser >> *bp::ws >> bp::eoi;
//
// BOOST_PARSER_DEFINE_RULES(unitParser);
// BOOST_PARSER_DEFINE_RULES(functionParser);
// BOOST_PARSER_DEFINE_RULES(statementParser);
// BOOST_PARSER_DEFINE_RULES(expressionParser);
const auto unitParser = identParser[([](auto& ctx) {
    return ast::Unit {
        .unitName = _attr(ctx)
    };
})];
} // namespace

ParserResult parse(const std::string_view source) noexcept {
    std::vector<ParseError> errors;
    auto errorHandler = callback_error_handler {[&errors](const std::string& error)  {
        errors.emplace_back(error, 0, 0);
    }};

    const auto parser = with_error_handler(unitParser, errorHandler);

    if (const auto result = parse(source, parser, ws)) {
        ast::Unit unit;
        //unit.unitName = result.value();
        return unit;
    }

    return std::unexpected(std::move(errors));
}

} // namespace skarn::lexer
