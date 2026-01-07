#pragma once

#include "ParserContext.h"
#include <memory>

namespace skarn::parser::details {

template <class Value, class Input = char>
requires (!std::is_same_v<Input, AnyInputType>)
class ParserStorage final {
    struct Deleter
    {
        void (*deleter)(void*) noexcept {};

        void operator()(void* const ptr) const noexcept
        {
            deleter(ptr);
        }
    };

    template <class Parser>
    static void deleter(void* const ptr) noexcept
    {
        std::destroy_at(static_cast<Parser*>(ptr));
        ::operator delete[](ptr, std::align_val_t {alignof(Parser)});
    }

    template <class Parser>
    std::unique_ptr<char[], Deleter> create(Parser parser) {
        void* ptr = ::operator new[](sizeof(Parser), std::align_val_t {alignof(Parser)});
        std::construct_at(static_cast<Parser*>(ptr), std::move(parser));
        return std::unique_ptr<char[], Deleter> {static_cast<char*>(ptr), Deleter {&deleter<Parser>}};
    }

    template <class Parser>
    static bool call_parse(const void* const ptr, ParserContext<Input>& ctx, Value& value)
    {
        return static_cast<const Parser*>(ptr)->parse(ctx, value);
    }

    template <class Parser>
    static bool call_parse_no_value(const void* const ptr, ParserContext<Input>& ctx)
    {
        return static_cast<const Parser*>(ptr)->parse(ctx);
    }

    std::unique_ptr<char[], Deleter> storage_;
    void* parser_ptr_ {};
    bool (*parse_ptr_)(const void* ptr, ParserContext<Input>& ctx, Value& value) {};
    bool (*parse_no_value_ptr_)(const void* ptr, ParserContext<Input>& ctx) {};

public:
    explicit ParserStorage() = default;

    template <Parser Parser>
    requires (std::is_same_v<Value, typename Parser::ValueType> && std::is_same_v<Input, typename Parser::InputType>)
    explicit ParserStorage(Parser parser)
        : storage_ {create(std::move(parser))}
        , parser_ptr_ {storage_.get()}
        , parse_ptr_ {&call_parse<Parser>}
        , parse_no_value_ptr_ {&call_parse_no_value<Parser>}
    {
    }

    [[nodiscard]] bool initialized() const noexcept {
        return parser_ptr_ != nullptr;
    }

    bool parse(ParserContext<Input>& ctx, Value& value) const {
        return parse_ptr_(parser_ptr_, ctx, value);
    }

    bool parse(ParserContext<Input>& ctx) const {
        return parse_no_value_ptr_(parser_ptr_, ctx);
    }
};

template <Parser Parser>
requires (!std::is_same_v<typename Parser::InputType, AnyInputType>)
ParserStorage(Parser parser) -> ParserStorage<typename Parser::ValueType, typename Parser::InputType>;

/// Parser that references to another parser, allowing to build recursive parser definitions.
template <class Value, class Input = char, class = decltype([]{})>
class ReferenceParser final {
    inline static ParserStorage<Value, Input> storage_;

public:
    using ParserType = ReferenceParser;
    using InputType = Input;
    using ValueType = Value;

    explicit constexpr ReferenceParser() noexcept = default;

    template <Parser Parser>
    requires (std::is_same_v<Value, typename Parser::ValueType> && std::is_same_v<Input, typename Parser::InputType>)
    void assign(Parser parser) const {
        std::ignore = this;
        storage_ = ParserStorage {parser};
    }

    bool parse(ParserContext<InputType>& ctx, ValueType& value) const
    requires (!std::is_same_v<ValueType, NoValueType>) {
        if (!storage_.initialized()) {
            throw std::logic_error {"Parser is not assigned"};
        }

        return storage_.parse(ctx, value);
    }

    bool parse(ParserContext<InputType>& ctx) const {
        if (!storage_.initialized()) {
            throw std::logic_error {"Parser is not assigned"};
        }

        return storage_.parse(ctx);
    }
};

} // namespace skarn::parser::details
