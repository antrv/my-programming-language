#pragma once

#include <type_traits>

namespace skarn::parser::details {

template <class T, template <class...> class Template>
inline constexpr bool isSpecializationOf = false;

template <template <class...> class Template, class... Args>
inline constexpr bool isSpecializationOf<Template<Args...>, Template> = true;

template <class T, class...Ts>
inline constexpr bool areAllSameTypesAs = (std::is_same_v<T, Ts> && ...);

template <class>
inline constexpr bool alwaysFalse = false;

template <class...Ts>
struct TypePack;

template <size_t Index, class Pack>
struct TypePackElement;

template <size_t Index>
struct TypePackElement<Index, TypePack<>> {
    static_assert(alwaysFalse<std::integral_constant<size_t, Index>>, "Index out of range");
};

template <size_t Index, class T, class...Ts>
struct TypePackElement<Index, TypePack<T, Ts...>> :
    std::type_identity<std::conditional_t<Index == 0, T, TypePackElement<Index - 1, TypePack<Ts...>>>> {
};

template <class...Ts>
struct TypePack : std::type_identity<TypePack<Ts...>> {
    static constexpr size_t size = sizeof...(Ts);

    template <size_t Index>
    using Element = TypePackElement<Index, TypePack>::type;
};

template <class Pack>
using TypePackHead = TypePackElement<0, Pack>;

} // namespace skarn::parser::details
