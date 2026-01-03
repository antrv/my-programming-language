#include <gtest/gtest.h>
#include "InvocableTraits.h"

using namespace skarn;

TEST(InvocableTraitsTests, Lambda)
{
    using lambda_t = decltype([](int&&, long*, const double&, char**, unsigned) {
        return u8'a';
    });

    static_assert(std::is_same_v<invocable_result_t<lambda_t>, char8_t>);
    static_assert(std::is_same_v<invocable_args_t<lambda_t>, TypePack<int&&, long*, const double&, char**, unsigned>>);
    static_assert(invocable_const_v<lambda_t>);
    static_assert(!invocable_noexcept_v<lambda_t>);
    static_assert(!invocable_static_v<lambda_t>);
}

TEST(InvocableTraitsTests, NoexceptLambda)
{
    using lambda_t = decltype([](int&&, long*, double, char**) noexcept {
        return u8'a';
    });

    static_assert(std::is_same_v<invocable_result_t<lambda_t>, char8_t>);
    static_assert(std::is_same_v<invocable_args_t<lambda_t>, TypePack<int&&, long*, double, char**>>);
    static_assert(invocable_const_v<lambda_t>);
    static_assert(invocable_noexcept_v<lambda_t>);
    static_assert(!invocable_static_v<lambda_t>);
}

TEST(InvocableTraitsTests, MutableLambda)
{
    using lambda_t = decltype([](int&&, long*, double, char**) mutable {
        return u8'a';
    });

    static_assert(std::is_same_v<invocable_result_t<lambda_t>, char8_t>);
    static_assert(std::is_same_v<invocable_args_t<lambda_t>, TypePack<int&&, long*, double, char**>>);
    static_assert(!invocable_const_v<lambda_t>);
    static_assert(!invocable_noexcept_v<lambda_t>);
    static_assert(!invocable_static_v<lambda_t>);
}

TEST(InvocableTraitsTests, StaticLambda)
{
    using lambda_t = decltype([](int&&, long*, double, char**) static noexcept {
        return u8'a';
    });

    static_assert(std::is_same_v<invocable_result_t<lambda_t>, char8_t>);
    static_assert(std::is_same_v<invocable_args_t<lambda_t>, TypePack<int&&, long*, double, char**>>);
    static_assert(!invocable_const_v<lambda_t>);
    static_assert(invocable_noexcept_v<lambda_t>);
    static_assert(invocable_static_v<lambda_t>);
}

namespace {
char8_t f1(int&&, long*, double, char**) {
    return u8'a';
}

char8_t f2(int&&, long*, double, char**) noexcept {
    return u8'a';
}
} // namespace

TEST(InvocableTraitsTests, Function)
{
    using function_t = decltype(f1);

    static_assert(std::is_same_v<invocable_result_t<function_t>, char8_t>);
    static_assert(std::is_same_v<invocable_args_t<function_t>, TypePack<int&&, long*, double, char**>>);
    static_assert(!invocable_const_v<function_t>);
    static_assert(!invocable_noexcept_v<function_t>);
    static_assert(invocable_static_v<function_t>);
}

TEST(InvocableTraitsTests, FunctionPointer)
{
    using function_t = decltype(&f1);

    static_assert(std::is_same_v<invocable_result_t<function_t>, char8_t>);
    static_assert(std::is_same_v<invocable_args_t<function_t>, TypePack<int&&, long*, double, char**>>);
    static_assert(!invocable_const_v<function_t>);
    static_assert(!invocable_noexcept_v<function_t>);
    static_assert(invocable_static_v<function_t>);
}

TEST(InvocableTraitsTests, NoexceptFunction)
{
    using lambda_t = decltype(f2);

    static_assert(std::is_same_v<invocable_result_t<lambda_t>, char8_t>);
    static_assert(std::is_same_v<invocable_args_t<lambda_t>, TypePack<int&&, long*, double, char**>>);
    static_assert(!invocable_const_v<lambda_t>);
    static_assert(invocable_noexcept_v<lambda_t>);
    static_assert(invocable_static_v<lambda_t>);
}

TEST(InvocableTraitsTests, NoexceptFunctionPointer)
{
    using lambda_t = decltype(&f2);

    static_assert(std::is_same_v<invocable_result_t<lambda_t>, char8_t>);
    static_assert(std::is_same_v<invocable_args_t<lambda_t>, TypePack<int&&, long*, double, char**>>);
    static_assert(!invocable_const_v<lambda_t>);
    static_assert(invocable_noexcept_v<lambda_t>);
    static_assert(invocable_static_v<lambda_t>);
}
