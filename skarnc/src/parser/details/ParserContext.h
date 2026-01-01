#pragma once

#include "../ParserMessage.h"
#include "TypePack.h"
#include <cstdint>
#include <format>
#include <span>
#include <stack>

namespace skarn::parser::details {

template <class Input>
class ParserContext final {
    std::span<const Input> input_;
    ParseMessages messages_;

    struct State {
        size_t position;
        uint32_t line;
        uint32_t column;
        bool reportMessages;
    };

    std::stack<State> states_;
    State current_;

public:
    explicit ParserContext(const std::span<const Input> input) noexcept
        : input_ {input}
        , current_ {0, 1, 1, true} {
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

    void report_messages(const bool value = true) noexcept {
        current_.reportMessages = value;
    }

    void push_state() {
        states_.push(current_);
    }

    void restore_state() {
        current_ = states_.top();
        states_.pop();
    }

    void pop_state() {
        states_.pop();
    }

    template <class...Args>
    void addMsg(const ParserMsgLevel level, const ParserMsgCode code, std::format_string<Args...> fmt, Args&&...args) {
        if (current_.reportMessages) {
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

struct AnyInputType final {
    AnyInputType() = delete;
};

template <class T>
concept Parser = std::is_same_v<typename T::InputType, AnyInputType> ||
    // TODO: check parse method signature in the case of InputType is AnyInputType
    requires(const T& p, ParserContext<typename T::InputType>& c, typename T::ValueType& v) {
        { p.parse(c, v) } -> std::same_as<bool>;
    };

template <class...Parsers>
concept CompatibleParsers = (Parser<Parsers> && ...) &&
    TypePack<typename Parsers::InputType...>::template remove_t<AnyInputType>::all_same;

template <Parser...Parsers>
using InputTypeOf = TypePack<typename Parsers::InputType...>::
    template remove_t<AnyInputType>::
    template insert_last_t<AnyInputType>::
    template element_t<0>;

} // namespace skarn::parser::details
