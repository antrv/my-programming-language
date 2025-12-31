#pragma once

#include "TypeTraits.h"
#include <limits>

namespace skarn {

template <class...Ts>
struct TypePack;

namespace details {
template <size_t Index, class Pack>
struct TypePackElement;

template <size_t Index>
struct TypePackElement<Index, TypePack<>> {
    static_assert(AlwaysFalse<std::integral_constant<size_t, Index>>, "Index out of range");
};

template <class T, class...Ts>
struct TypePackElement<0, TypePack<T, Ts...>> : std::type_identity<T> {
};

template <size_t Index, class T, class...Ts>
requires (Index > 0 && Index <= sizeof...(Ts))
struct TypePackElement<Index, TypePack<T, Ts...>> :
    std::type_identity<typename TypePackElement<Index - 1, TypePack<Ts...>>::type> {
};

template <class T, class Pack>
struct TypePackInsertAtFirstPosition;

template <class T, class...Ts>
struct TypePackInsertAtFirstPosition<T, TypePack<Ts...>> : std::type_identity<TypePack<T, Ts...>> {
};

template <size_t Size, class Pack>
struct TypePackFirstN;

template <size_t Size>
requires (Size != 0)
struct TypePackFirstN<Size, TypePack<>> {
    static_assert(AlwaysFalse<std::integral_constant<size_t, Size>>, "Size out of range");
};

template <class...Ts>
struct TypePackFirstN<0, TypePack<Ts...>> : std::type_identity<TypePack<>> {
};

template <size_t Size, class T, class...Ts>
requires (Size != 0)
struct TypePackFirstN<Size, TypePack<T, Ts...>> :
    std::type_identity<typename TypePackInsertAtFirstPosition<T, typename TypePackFirstN<Size - 1, TypePack<Ts...>>::type>::type> {
};

template <size_t Size, class Pack>
struct TypePackSkipN;

template <size_t Size>
requires (Size != 0)
struct TypePackSkipN<Size, TypePack<>> {
    static_assert(AlwaysFalse<std::integral_constant<size_t, Size>>, "Size out of range");
};

template <class...Ts>
struct TypePackSkipN<0, TypePack<Ts...>> : std::type_identity<TypePack<Ts...>> {
};

template <size_t Size, class T, class...Ts>
requires (Size != 0)
struct TypePackSkipN<Size, TypePack<T, Ts...>> :
    std::type_identity<typename TypePackSkipN<Size - 1, TypePack<Ts...>>::type> {
};

template <size_t Index, size_t Size, class Pack>
struct TypePackSubPack;

template <size_t Index, size_t Size, class...Ts>
requires (Index > sizeof...(Ts) || Size > sizeof...(Ts) || Index + Size > sizeof...(Ts))
struct TypePackSubPack<Index, Size, TypePack<Ts...>> {
    static_assert(Index <= sizeof...(Ts), "Index out of range");
    static_assert(Size <= sizeof...(Ts), "Size out of range");
    static_assert(Index + Size <= sizeof...(Ts), "Index + Size out of range");
};

template <size_t Index, size_t Size, class...Ts>
requires (Index <= sizeof...(Ts) && Size <= sizeof...(Ts) && Index + Size <= sizeof...(Ts))
struct TypePackSubPack<Index, Size, TypePack<Ts...>> :
    TypePackFirstN<Size, typename TypePackSkipN<Index, TypePack<Ts...>>::type> {
};

template <IsSpecializationOf<TypePack>...Packs>
struct TypePackConcat;

template <>
struct TypePackConcat<> : std::type_identity<TypePack<>> {
};

template <class...Ts>
struct TypePackConcat<TypePack<Ts...>> : std::type_identity<TypePack<Ts...>> {
};

template <class...Ts, class...Ts2, IsSpecializationOf<TypePack>...Packs>
struct TypePackConcat<TypePack<Ts...>, TypePack<Ts2...>, Packs...> :
    std::type_identity<typename TypePackConcat<TypePack<Ts..., Ts2...>, Packs...>::type> {
};

template <size_t Index, class Pack, class...Ts2>
struct TypePackInsertAt;

template <size_t Index, class...Ts, class...Ts2>
requires (Index > sizeof...(Ts))
struct TypePackInsertAt<Index, TypePack<Ts...>, Ts2...> {
    static_assert(AlwaysFalse<std::integral_constant<size_t, Index>>, "Index out of range");
};

template <size_t Index, class...Ts, class...Ts2>
requires (Index <= sizeof...(Ts))
struct TypePackInsertAt<Index, TypePack<Ts...>, Ts2...> :
    TypePackConcat<typename TypePackFirstN<Index, TypePack<Ts...>>::type, TypePack<Ts2...>, typename TypePackSkipN<Index, TypePack<Ts...>>::type> {
};

template <size_t Index, size_t Size, class Pack>
struct TypePackRemoveRange;

template <size_t Index, size_t Size, class...Ts>
requires (Index > sizeof...(Ts) || Size > sizeof...(Ts) || Index + Size > sizeof...(Ts))
struct TypePackRemoveRange<Index, Size, TypePack<Ts...>> {
    static_assert(Index <= sizeof...(Ts), "Index out of range");
    static_assert(Size <= sizeof...(Ts), "Size out of range");
    static_assert(Index + Size <= sizeof...(Ts), "Index + Size out of range");
};

template <size_t Index, size_t Size, class...Ts>
requires (Index <= sizeof...(Ts) && Size <= sizeof...(Ts) && Index + Size <= sizeof...(Ts))
struct TypePackRemoveRange<Index, Size, TypePack<Ts...>> :
    TypePackConcat<typename TypePackFirstN<Index, TypePack<Ts...>>::type, typename TypePackSkipN<Index + Size, TypePack<Ts...>>::type> {
};

template <class T>
struct TypePackFlatten : std::type_identity<TypePack<T>> {
};

template <class...Ts>
struct TypePackFlatten<TypePack<Ts...>> : TypePackConcat<typename TypePackFlatten<Ts>::type...> {
};

template <class Pack, class...Ts2>
struct TypePackRemove;

template <class...Ts2>
struct TypePackRemove<TypePack<>, Ts2...> : std::type_identity<TypePack<>> {
};

template <class TFirst, class...Ts, class...Ts2>
requires (std::is_same_v<TFirst, Ts2> || ...)
struct TypePackRemove<TypePack<TFirst, Ts...>, Ts2...> : TypePackRemove<TypePack<Ts...>, Ts2...> {
};

template <class TFirst, class...Ts, class...Ts2>
requires (!std::is_same_v<TFirst, Ts2> && ...)
struct TypePackRemove<TypePack<TFirst, Ts...>, Ts2...> :
    std::type_identity<typename TypePackInsertAtFirstPosition<TFirst, typename TypePackRemove<TypePack<Ts...>, Ts2...>::type>::type> {
};

template <class T, class TReplacement, class Pack>
struct TypePackReplace;

template <class T, class TReplacement>
struct TypePackReplace<T, TReplacement, TypePack<>> : std::type_identity<TypePack<>> {
};

template <class T, class TReplacement, class...Ts>
struct TypePackReplace<T, TReplacement, TypePack<T, Ts...>> :
    std::type_identity<typename TypePackInsertAtFirstPosition<TReplacement, typename TypePackReplace<T, TReplacement, TypePack<Ts...>>::type>::type> {
};

template <class T, class TReplacement, class TFirst, class...Ts>
struct TypePackReplace<T, TReplacement, TypePack<TFirst, Ts...>> :
    std::type_identity<typename TypePackInsertAtFirstPosition<TFirst, typename TypePackReplace<T, TReplacement, TypePack<Ts...>>::type>::type> {
};

template <class T, class Pack>
struct TypePackIndexOf;

template <class T>
struct TypePackIndexOf<T, TypePack<>> : std::integral_constant<size_t, std::numeric_limits<size_t>::max()> {
};

template <class T, class...Ts>
struct TypePackIndexOf<T, TypePack<T, Ts...>> : std::integral_constant<size_t, 0> {
};

template <class T, class TFirst, class...Ts>
struct TypePackIndexOf<T, TypePack<TFirst, Ts...>> :
    std::integral_constant<size_t,
        TypePackIndexOf<T, TypePack<Ts...>>::value == std::numeric_limits<size_t>::max() ?
            std::numeric_limits<size_t>::max() : TypePackIndexOf<T, TypePack<Ts...>>::value + 1> {
};

} // namespace details

template <class...Ts>
struct TypePack : std::type_identity<TypePack<Ts...>> {
    static constexpr size_t size = sizeof...(Ts);

    template <size_t Index>
    using element_t = details::TypePackElement<Index, TypePack>::type;

    template <size_t Size>
    using first_n_t = details::TypePackFirstN<Size, TypePack>::type;

    template <size_t Size>
    using skip_n_t = details::TypePackSkipN<Size, TypePack>::type;

    template <size_t Index, size_t Size>
    using subpack_t = details::TypePackSubPack<Index, Size, TypePack>::type;

    template <class...Packs>
    using concat_t = details::TypePackConcat<TypePack, Packs...>::type;

    template <size_t Index, class...Ts2>
    using insert_at_t = details::TypePackInsertAt<Index, TypePack, Ts2...>::type;

    template <size_t Index>
    using remove_at_t = details::TypePackRemoveRange<Index, 1, TypePack>::type;

    template <size_t Index, size_t Size>
    using remove_range_t = details::TypePackRemoveRange<Index, Size, TypePack>::type;

    template <class...Ts2>
    using remove_t = details::TypePackRemove<TypePack, Ts2...>::type;

    template <class T, class TReplacement>
    using replace_t = details::TypePackReplace<T, TReplacement, TypePack>::type;

    template <class T>
    static constexpr bool contains = (std::is_same_v<T, Ts> || ...);

    template <class T>
    static constexpr size_t index_of = details::TypePackIndexOf<T, TypePack>::value;
};

template <IsSpecializationOf<TypePack>...Packs>
using type_pack_concat_t = details::TypePackConcat<Packs...>::type;

template <IsSpecializationOf<TypePack> Pack>
using type_pack_flatten_t = details::TypePackFlatten<Pack>::type;

} // namespace skarn
