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

    struct State {
        ParseMessages messages;
        size_t position {};
        uint32_t line {1};
        uint32_t column {1};
        bool report_messages {true};
    };

    std::stack<State> states_;
    State current_;

public:
    explicit ParserContext(const std::span<const Input> input) noexcept
        : input_ {input} {
    }

    [[nodiscard]] std::span<const Input> input() const {
        return input_.subspan(current_.position);
    }

    void consume(const size_t length) noexcept {
        // TODO: calculate line and column
        current_.position += length;
        current_.column += static_cast<uint32_t>(length);
    }

    [[nodiscard]] ParseMessages& messages() & noexcept {
        return current_.messages;
    }

    [[nodiscard]] const ParseMessages& messages() const & noexcept {
        return current_.messages;
    }

    [[nodiscard]] ParseMessages messages() && noexcept {
        return std::move(current_.messages);
    }

    [[nodiscard]] bool report_messages() const noexcept {
        return current_.report_messages;
    }

    void report_messages(const bool value) noexcept {
        current_.report_messages = value;
    }

    template <class...Args>
    void add_message(const ParserMsgLevel level, const ParserMsgCode code, std::format_string<Args...> fmt, Args&&...args) {
        if (current_.report_messages) {
            current_.messages.push_back(ParserMessage {
                .level = level,
                .code = code,
                .expected = std::format(fmt, std::forward<Args>(args)...),
                .position = current_.position,
                .line = current_.line,
                .column = current_.column,
            });
        }
    }

    void save_state() {
        states_.push(std::move(current_));
    }

    void rollback_state() {
        State& state = states_.top();
        current_ = std::move(state);
        states_.pop();
    }

    void commit_state() {
        State& state = states_.top();
        current_.messages.swap(state.messages);
        current_.messages.append_range(std::move(state.messages));
        states_.pop();
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
