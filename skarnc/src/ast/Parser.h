#pragma once

#include "ast/Unit.h"
#include "parser/Parser.h"

namespace skarn::ast {

consteval auto create_parser() noexcept {
    using namespace std::string_view_literals;
    using namespace skarn::parser;

    constexpr auto ws = Parse::char_([](const char c) constexpr static noexcept {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    });

    constexpr auto ws_many = *ws;
    constexpr auto ws_at_least_one = +ws;

    constexpr auto ident = Parse::char_([](const char c) constexpr static noexcept {
            return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
        }, "identifier"sv) >>
        *Parse::char_([](const char c) constexpr static noexcept {
            return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c >= '0' && c <= '9' || c == '_';
        }, "identifier"sv) >>
        [](std::string& result, const std::tuple<char, std::string>& value) static {
            result = std::get<0>(value) + std::get<1>(value);
        };

    constexpr auto variableDeclaration =
        "let"sv >> ws_at_least_one >> ident >> ws_many >> '=' >> ws_many >> expression;

    constexpr auto variableAssignement = ident >> ws_many >> '=' >> ws_many >> expression;

    constexpr auto whileLoop =
        "while"sv >> ws_at_least_one >> expression >> ws_many >>
        '{' >> ws_many >> *statement >> ws_many >> '}';

    constexpr auto statement = variableDeclaration.wrap(ws_many) || variableAssignement.wrap(ws_many) || whileLoop;

    constexpr auto function =
        "fn"sv >> ws_at_least_one >>
        ident >> ws_many >>
        '(' >> ws_many >> ident.seq(ws_many >> ',' >> ws_many) >> ws_many >> ')' >> ws_many >>
        '{' >> ws_many >> *statement >> ws_many >> '}';

    constexpr auto unitParser = *function.wrap(ws_many) >>
        [](Unit& unit, std::vector<Function>& functions) static noexcept {
            unit.functions = std::move(functions);
        };

    return unitParser;
}

} // namespace skarn::ast
