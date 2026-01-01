#pragma once

#include <type_traits>

namespace skarn {

namespace details {
template <class...>
inline constexpr bool always_false = false;

template <class, template <class...> class>
inline constexpr bool specialization_of = false;

template <template <class...> class Template, class...Ts>
inline constexpr bool specialization_of<Template<Ts...>, Template> = true;

template <class...>
inline constexpr bool all_same_types = false;

template <>
inline constexpr bool all_same_types<> = true;

template <class T, class...Ts>
inline constexpr bool all_same_types<T, Ts...> = (std::is_same_v<T, Ts> && ...);
} // namespace details

template <class T>
concept AlwaysFalse = details::always_false<T>;

template <class T, template <class...> class Template>
concept SpecializationOf = details::specialization_of<T, Template>;

template <class...Ts>
concept AllSameTypes = details::all_same_types<Ts...>;

template <class T, class...Ts>
concept AllSameTypeAs = details::all_same_types<T, Ts...>;

} // namespace skarn
