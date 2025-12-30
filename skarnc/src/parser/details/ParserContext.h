#pragma once

#include "Concepts.h"
#include "../ParserMessage.h"
#include <cstdint>
#include <format>
#include <span>
#include <stack>

namespace skarn::parser::details {

template <class Input>
class ParserContext final {
    std::span<const Input> input_;
    ParseMessages messages_;
    bool reportMessages_ = true;

    struct Position {
        size_t position;
        uint32_t line;
        uint32_t column;
    };

    std::stack<Position> positions_;
    Position current_;

public:
    explicit ParserContext(const std::span<const Input> input) noexcept
        : input_ {input}
        , current_ {0, 1, 1} {
    }

    [[nodiscard]] std::span<const Input> input() const {
        return input_.subspan(current_.position);
    }

    void consume(const size_t length) noexcept {
        // TODO: calculate line and column
        current_.position += length;
        current_.column += static_cast<uint32_t>(length);
    }

    [[nodiscard]] const ParseMessages& messages() const & noexcept {
        return messages_;
    }

    [[nodiscard]] ParseMessages messages() && noexcept {
        return std::move(messages_);
    }

    template <class...Args>
    void addMsg(const ParserMsgLevel level, const ParserMsgCode code, std::format_string<Args...> fmt, Args&&...args) {
        if (reportMessages_) {
            messages_.push_back(ParserMessage {
                .level = level,
                .code = code,
                .message = std::format(fmt, std::forward<Args>(args)...),
                .position = current_.position,
                .line = current_.line,
                .column = current_.column,
            });
        }
    }
};

struct AnyInputType final {};

template <class T>
concept IsParser = std::is_same_v<T, typename T::ParserType> &&
    requires(const T& p, ParserContext<typename T::InputType>& c, typename T::ValueType& v) {
        { p.parse(c, v) } -> std::same_as<bool>;
    };

template <class...Parsers>
concept CompatibleParsers = areAllSameTypesAs<typename Parsers::InputType...>;

} // namespace skarn::parser::details
