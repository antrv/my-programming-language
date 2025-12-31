#pragma once

#include <type_traits>

namespace skarn {

namespace details {
template <class>
inline constexpr bool alwaysFalse = false;

template <class, template <class...> class>
inline constexpr bool isSpecializationOf = false;

template <template <class...> class Template, class... Ts>
inline constexpr bool isSpecializationOf<Template<Ts...>, Template> = true;

template <class T, class...Ts>
inline constexpr bool areAllSameTypesAs = (std::is_same_v<T, Ts> && ...);
} // namespace details

template <class T>
concept AlwaysFalse = details::alwaysFalse<T>;

template <class T, template <class...> class Template>
concept IsSpecializationOf = details::isSpecializationOf<T, Template>;

template <class...Ts>
concept AllSameTypes = details::areAllSameTypesAs<Ts...>;

template <class T, class...Ts>
concept AllSameTypeAs = details::areAllSameTypesAs<T, Ts...>;

} // namespace skarn
