#pragma once

#include "../ParserMessage.h"
#include "TypePack.h"
#include <cstdint>
#include <format>
#include <span>

namespace skarn::parser::details {

struct ParserPosition {
    size_t offset;
    uint32_t line;
    uint32_t column;
};

template <class Input>
class ParserContext final {
    ParseMessages messages_;
    std::span<const Input> input_;
    ParserPosition position_ {0, 1, 1};
    bool report_messages_ {true};

public:
    explicit ParserContext(const std::span<const Input> input) noexcept
        : input_ {input} {
    }

    [[nodiscard]] std::span<const Input> input() const {
        return input_.subspan(position_.offset);
    }

    [[nodiscard]] ParserPosition position() const noexcept {
        return position_;
    }

    void position(const ParserPosition position) noexcept {
        position_ = position;
    }

    void consume(const size_t length) noexcept {
        // TODO: calculate line and column
        position_.offset += length;
        position_.column += static_cast<uint32_t>(length);
    }

    [[nodiscard]] ParseMessages& messages() & noexcept {
        return messages_;
    }

    [[nodiscard]] const ParseMessages& messages() const & noexcept {
        return messages_;
    }

    [[nodiscard]] ParseMessages messages() && noexcept {
        return std::move(messages_);
    }

    [[nodiscard]] bool report_messages() const noexcept {
        return report_messages_;
    }

    void report_messages(const bool value) noexcept {
        report_messages_ = value;
    }

    template <class...Args>
    void add_message(const ParserMsgLevel level, const ParserMsgCode code, std::format_string<Args...> fmt, Args&&...args) {
        if (report_messages_) {
            messages_.push_back(ParserMessage {
                .level = level,
                .code = code,
                .expected = std::format(fmt, std::forward<Args>(args)...),
                .offset = position_.offset,
                .line = position_.line,
                .column = position_.column,
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
