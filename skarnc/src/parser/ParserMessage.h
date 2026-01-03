#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace skarn::parser {

enum class ParserMsgLevel {
    Fatal,
    Error,
    Warning,
    Info,
    Hint,
};

enum class ParserMsgCode {
    C0000,
    C0001, // Unexpected end of input
    C0002, // Unexpected input
    C0003, // Out of range
};

struct ParserMessage {
    ParserMsgLevel level;
    ParserMsgCode code;
    std::string expected;
    size_t offset;
    uint32_t line;
    uint32_t column;
};

using ParseMessages = std::vector<ParserMessage>;

} // namespace skarn::parser
