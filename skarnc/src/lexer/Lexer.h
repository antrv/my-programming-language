#pragma once

#include <expected>
#include <string>
#include <vector>

#include "ast/Unit.h"

namespace skarn::lexer {

struct ParseError {
    std::string message;
    size_t line;
    size_t position;
};

using ParserResult = std::expected<ast::Unit, std::vector<ParseError>>;

ParserResult parse(std::string_view source) noexcept;

} // namespace skarn::lexer
