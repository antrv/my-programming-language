#pragma once

#include "TypePack.h"

namespace skarn {

namespace details {
template <class>
struct Invocable;

template <class Result, class Type, class...Args>
struct Invocable<Result(Type::*)(Args...)> : std::type_identity<Type> {
    static constexpr bool is_static = false;
    static constexpr bool is_const = false;
    static constexpr bool is_noexcept = false;
    using result_t = Result;
    using args_t = TypePack<Args...>;
};

template <class Result, class Type, class...Args>
struct Invocable<Result(Type::*)(Args...) noexcept> : std::type_identity<Type> {
    static constexpr bool is_static = false;
    static constexpr bool is_const = false;
    static constexpr bool is_noexcept = true;
    using result_t = Result;
    using args_t = TypePack<Args...>;
};

template <class Result, class Type, class...Args>
struct Invocable<Result(Type::*)(Args...) const> {
    static constexpr bool is_static = false;
    static constexpr bool is_const = true;
    static constexpr bool is_noexcept = false;
    using result_t = Result;
    using args_t = TypePack<Args...>;
};

template <class Result, class Type, class...Args>
struct Invocable<Result(Type::*)(Args...) const noexcept> {
    static constexpr bool is_static = false;
    static constexpr bool is_const = true;
    static constexpr bool is_noexcept = true;
    using result_t = Result;
    using args_t = TypePack<Args...>;
};

template <class Result, class...Args>
struct Invocable<Result(*)(Args...)> {
    static constexpr bool is_static = true;
    static constexpr bool is_const = false;
    static constexpr bool is_noexcept = false;
    using result_t = Result;
    using args_t = TypePack<Args...>;
};

template <class Result, class...Args>
struct Invocable<Result(*)(Args...) noexcept> {
    static constexpr bool is_static = true;
    static constexpr bool is_const = false;
    static constexpr bool is_noexcept = true;
    using result_t = Result;
    using args_t = TypePack<Args...>;
};

template <class Result, class...Args>
struct Invocable<Result(Args...)> : Invocable<Result(*)(Args...)> {
};

template <class Result, class...Args>
struct Invocable<Result(Args...) noexcept> : Invocable<Result(*)(Args...) noexcept> {
};

template <class F>
requires (requires { &F::operator(); })
struct Invocable<F> : Invocable<decltype(&F::operator())> {
};
} // namespace details

template <class F>
using invocable_result_t = details::Invocable<F>::result_t;

template <class F>
using invocable_args_t = details::Invocable<F>::args_t;

template <class F>
inline constexpr bool invocable_static_v = details::Invocable<F>::is_static;

template <class F>
inline constexpr bool invocable_const_v = details::Invocable<F>::is_const;

template <class F>
inline constexpr bool invocable_noexcept_v = details::Invocable<F>::is_noexcept;

} // namespace skarn
