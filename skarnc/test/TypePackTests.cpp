#include <gtest/gtest.h>
#include "TypePack.h"
#include <tuple>

using namespace skarn;

TEST(TypePackTests, ElementType)
{
    using Pack = TypePack<int, long, double, char>;
    static_assert(Pack::size == 4);
    static_assert(std::is_same_v<Pack::type, Pack>);
    static_assert(std::is_same_v<Pack::element_t<0>, int>);
    static_assert(std::is_same_v<Pack::element_t<1>, long>);
    static_assert(std::is_same_v<Pack::element_t<2>, double>);
    static_assert(std::is_same_v<Pack::element_t<3>, char>);
}

TEST(TypePackTests, FirstN0)
{
    using Pack = TypePack<>;
    static_assert(std::is_same_v<Pack::first_n_t<0>, TypePack<>>);
}

TEST(TypePackTests, FirstN1)
{
    using Pack = TypePack<int>;
    static_assert(std::is_same_v<Pack::first_n_t<0>, TypePack<>>);
    static_assert(std::is_same_v<Pack::first_n_t<1>, TypePack<int>>);
}

TEST(TypePackTests, FirstN)
{
    using Pack = TypePack<int, long, double, char>;
    static_assert(std::is_same_v<Pack::first_n_t<0>, TypePack<>>);
    static_assert(std::is_same_v<Pack::first_n_t<1>, TypePack<int>>);
    static_assert(std::is_same_v<Pack::first_n_t<2>, TypePack<int, long>>);
    static_assert(std::is_same_v<Pack::first_n_t<3>, TypePack<int, long, double>>);
    static_assert(std::is_same_v<Pack::first_n_t<4>, TypePack<int, long, double, char>>);
}

TEST(TypePackTests, SkipN0)
{
    using Pack = TypePack<>;
    static_assert(std::is_same_v<Pack::skip_n_t<0>, TypePack<>>);
}

TEST(TypePackTests, SkipN1)
{
    using Pack = TypePack<int>;
    static_assert(std::is_same_v<Pack::skip_n_t<0>, TypePack<int>>);
    static_assert(std::is_same_v<Pack::skip_n_t<1>, TypePack<>>);
}

TEST(TypePackTests, SkipN)
{
    using Pack = TypePack<int, long, double, char>;
    static_assert(std::is_same_v<Pack::skip_n_t<0>, TypePack<int, long, double, char>>);
    static_assert(std::is_same_v<Pack::skip_n_t<1>, TypePack<long, double, char>>);
    static_assert(std::is_same_v<Pack::skip_n_t<2>, TypePack<double, char>>);
    static_assert(std::is_same_v<Pack::skip_n_t<3>, TypePack<char>>);
    static_assert(std::is_same_v<Pack::skip_n_t<4>, TypePack<>>);
}

TEST(TypePackTests, SubPack0)
{
    using Pack = TypePack<>;
    static_assert(std::is_same_v<Pack::subpack_t<0, 0>, TypePack<>>);
}

TEST(TypePackTests, SubPack1)
{
    using Pack = TypePack<int>;
    static_assert(std::is_same_v<Pack::subpack_t<0, 0>, TypePack<>>);
    static_assert(std::is_same_v<Pack::subpack_t<0, 1>, TypePack<int>>);
    static_assert(std::is_same_v<Pack::subpack_t<1, 0>, TypePack<>>);
}

TEST(TypePackTests, SubPack)
{
    using Pack = TypePack<int, long, double, char>;
    static_assert(std::is_same_v<Pack::subpack_t<0, 0>, TypePack<>>);
    static_assert(std::is_same_v<Pack::subpack_t<0, 1>, TypePack<int>>);
    static_assert(std::is_same_v<Pack::subpack_t<1, 1>, TypePack<long>>);
    static_assert(std::is_same_v<Pack::subpack_t<2, 1>, TypePack<double>>);
    static_assert(std::is_same_v<Pack::subpack_t<3, 1>, TypePack<char>>);
    static_assert(std::is_same_v<Pack::subpack_t<0, 2>, TypePack<int, long>>);
    static_assert(std::is_same_v<Pack::subpack_t<1, 2>, TypePack<long, double>>);
    static_assert(std::is_same_v<Pack::subpack_t<2, 2>, TypePack<double, char>>);
    static_assert(std::is_same_v<Pack::subpack_t<0, 3>, TypePack<int, long, double>>);
    static_assert(std::is_same_v<Pack::subpack_t<1, 3>, TypePack<long, double, char>>);
    static_assert(std::is_same_v<Pack::subpack_t<0, 4>, TypePack<int, long, double, char>>);
}

TEST(TypePackTests, Concat)
{
    using Pack0 = TypePack<>;
    using Pack1 = TypePack<float>;
    using Pack2 = TypePack<unsigned, char8_t>;
    using Pack4 = TypePack<int, long, double, char>;
    static_assert(std::is_same_v<Pack0::concat_t<Pack0>, TypePack<>>);
    static_assert(std::is_same_v<Pack0::concat_t<Pack1>, TypePack<float>>);
    static_assert(std::is_same_v<Pack0::concat_t<Pack2>, TypePack<unsigned, char8_t>>);
    static_assert(std::is_same_v<Pack0::concat_t<Pack4>, TypePack<int, long, double, char>>);
    static_assert(std::is_same_v<Pack1::concat_t<Pack0>, TypePack<float>>);
    static_assert(std::is_same_v<Pack1::concat_t<Pack1>, TypePack<float, float>>);
    static_assert(std::is_same_v<Pack1::concat_t<Pack2>, TypePack<float, unsigned, char8_t>>);
    static_assert(std::is_same_v<Pack1::concat_t<Pack4>, TypePack<float, int, long, double, char>>);
    static_assert(std::is_same_v<Pack2::concat_t<Pack0>, TypePack<unsigned, char8_t>>);
    static_assert(std::is_same_v<Pack2::concat_t<Pack1>, TypePack<unsigned, char8_t, float>>);
    static_assert(std::is_same_v<Pack2::concat_t<Pack2>, TypePack<unsigned, char8_t, unsigned, char8_t>>);
    static_assert(std::is_same_v<Pack2::concat_t<Pack4>, TypePack<unsigned, char8_t, int, long, double, char>>);
    static_assert(std::is_same_v<Pack4::concat_t<Pack0>, TypePack<int, long, double, char>>);
    static_assert(std::is_same_v<Pack4::concat_t<Pack1>, TypePack<int, long, double, char, float>>);
    static_assert(std::is_same_v<Pack4::concat_t<Pack2>, TypePack<int, long, double, char, unsigned, char8_t>>);
    static_assert(std::is_same_v<Pack4::concat_t<Pack4>, TypePack<int, long, double, char, int, long, double, char>>);
}

TEST(TypePackTests, InsertAt)
{
    using Pack = TypePack<int, long, double, char>;
    static_assert(std::is_same_v<Pack::insert_at_t<0, float>, TypePack<float, int, long, double, char>>);
    static_assert(std::is_same_v<Pack::insert_at_t<1, float>, TypePack<int, float, long, double, char>>);
    static_assert(std::is_same_v<Pack::insert_at_t<2, float>, TypePack<int, long, float, double, char>>);
    static_assert(std::is_same_v<Pack::insert_at_t<3, float>, TypePack<int, long, double, float, char>>);
    static_assert(std::is_same_v<Pack::insert_at_t<4, float>, TypePack<int, long, double, char, float>>);

    static_assert(std::is_same_v<Pack::insert_at_t<2, float, unsigned>, TypePack<int, long, float, unsigned, double, char>>);
}

TEST(TypePackTests, RemoveAt)
{
    using Pack = TypePack<int, long, double, char>;
    static_assert(std::is_same_v<Pack::remove_at_t<0>, TypePack<long, double, char>>);
    static_assert(std::is_same_v<Pack::remove_at_t<1>, TypePack<int, double, char>>);
    static_assert(std::is_same_v<Pack::remove_at_t<2>, TypePack<int, long, char>>);
    static_assert(std::is_same_v<Pack::remove_at_t<3>, TypePack<int, long, double>>);
}

TEST(TypePackTests, RemoveRange)
{
    using Pack = TypePack<int, long, double, char>;
    static_assert(std::is_same_v<Pack::remove_range_t<0, 0>, TypePack<int, long, double, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<1, 0>, TypePack<int, long, double, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<2, 0>, TypePack<int, long, double, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<3, 0>, TypePack<int, long, double, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<4, 0>, TypePack<int, long, double, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<0, 1>, TypePack<long, double, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<1, 1>, TypePack<int, double, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<2, 1>, TypePack<int, long, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<3, 1>, TypePack<int, long, double>>);
    static_assert(std::is_same_v<Pack::remove_range_t<0, 2>, TypePack<double, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<1, 2>, TypePack<int, char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<2, 2>, TypePack<int, long>>);
    static_assert(std::is_same_v<Pack::remove_range_t<0, 3>, TypePack<char>>);
    static_assert(std::is_same_v<Pack::remove_range_t<1, 3>, TypePack<int>>);
    static_assert(std::is_same_v<Pack::remove_range_t<0, 4>, TypePack<>>);
}

TEST(TypePackTests, RemoveType0)
{
    using Pack = TypePack<>;
    static_assert(std::is_same_v<Pack::remove_t<int>, TypePack<>>);
}

TEST(TypePackTests, RemoveType1)
{
    using Pack = TypePack<int>;
    static_assert(std::is_same_v<Pack::remove_t<int>, TypePack<>>);
    static_assert(std::is_same_v<Pack::remove_t<long>, TypePack<int>>);
}

TEST(TypePackTests, RemoveType)
{
    using Pack = TypePack<int, long, int, double, char, double, char, double, int>;
    static_assert(std::is_same_v<Pack::remove_t<int>, TypePack<long, double, char, double, char, double>>);
    static_assert(std::is_same_v<Pack::remove_t<long>, TypePack<int, int, double, char, double, char, double, int>>);
    static_assert(std::is_same_v<Pack::remove_t<double>, TypePack<int, long, int, char, char, int>>);
    static_assert(std::is_same_v<Pack::remove_t<char>, TypePack<int, long, int, double, double, double, int>>);
    static_assert(std::is_same_v<Pack::remove_t<char8_t>, Pack>);
    static_assert(std::is_same_v<Pack::remove_t<char, int>, TypePack<long, double, double, double>>);
    static_assert(std::is_same_v<Pack::remove_t<char, int, double>, TypePack<long>>);
}

TEST(TypePackTests, ReplaceType0)
{
    using Pack = TypePack<>;
    static_assert(std::is_same_v<Pack::replace_t<int, unsigned>, TypePack<>>);
}

TEST(TypePackTests, ReplaceType1)
{
    using Pack = TypePack<int>;
    static_assert(std::is_same_v<Pack::replace_t<int, unsigned>, TypePack<unsigned>>);
    static_assert(std::is_same_v<Pack::replace_t<long, unsigned>, TypePack<int>>);
}

TEST(TypePackTests, ReplaceType)
{
    using Pack = TypePack<int, long, int, double, char>;
    static_assert(std::is_same_v<Pack::replace_t<int, unsigned>, TypePack<unsigned, long, unsigned, double, char>>);
    static_assert(std::is_same_v<Pack::replace_t<long, unsigned>, TypePack<int, unsigned, int, double, char>>);
    static_assert(std::is_same_v<Pack::replace_t<double, unsigned>, TypePack<int, long, int, unsigned, char>>);
    static_assert(std::is_same_v<Pack::replace_t<char, unsigned>, TypePack<int, long, int, double, unsigned>>);
    static_assert(std::is_same_v<Pack::replace_t<char8_t, unsigned>, TypePack<int, long, int, double, char>>);
}

TEST(TypePackTests, Flatten0)
{
    using Pack = TypePack<>;
    static_assert(std::is_same_v<type_pack_flatten_t<Pack>, TypePack<>>);
}

TEST(TypePackTests, Flatten1)
{
    using Pack = TypePack<int>;
    static_assert(std::is_same_v<type_pack_flatten_t<Pack>, TypePack<int>>);
    using Pack1 = TypePack<TypePack<int>>;
    static_assert(std::is_same_v<type_pack_flatten_t<Pack1>, TypePack<int>>);
    using Pack2 = TypePack<TypePack<TypePack<int>>>;
    static_assert(std::is_same_v<type_pack_flatten_t<Pack2>, TypePack<int>>);
}

TEST(TypePackTests, Flatten)
{
    using Pack = TypePack<short, TypePack<>, TypePack<TypePack<int, TypePack<TypePack<long>>>,
        TypePack<double>>, TypePack<char8_t, char32_t>, char>;

    static_assert(std::is_same_v<type_pack_flatten_t<Pack>, TypePack<short, int, long, double, char8_t, char32_t, char>>);
}

TEST(TypePackTests, Contains0)
{
    using Pack = TypePack<>;
    static_assert(!Pack::contains<int>);
    static_assert(!Pack::contains<long>);
    static_assert(!Pack::contains_any<int, long>);
    static_assert(!Pack::contains_all<int, long>);
}

TEST(TypePackTests, Contains1)
{
    using Pack = TypePack<int>;
    static_assert(Pack::contains<int>);
    static_assert(!Pack::contains<long>);
    static_assert(Pack::contains_any<int, long>);
    static_assert(!Pack::contains_any<float, double>);
    static_assert(Pack::contains_all<int>);
    static_assert(!Pack::contains_all<int, long>);
}

TEST(TypePackTests, Contains)
{
    using Pack = TypePack<int, long, double, char>;
    static_assert(Pack::contains<int>);
    static_assert(Pack::contains<long>);
    static_assert(Pack::contains<double>);
    static_assert(Pack::contains<char>);
    static_assert(!Pack::contains<char8_t>);
    static_assert(Pack::contains_any<int, unsigned>);
    static_assert(!Pack::contains_any<char8_t, char16_t>);
    static_assert(Pack::contains_all<int, char>);
    static_assert(!Pack::contains_all<int, char8_t>);
}

TEST(TypePackTests, IndexOf0)
{
    using Pack = TypePack<>;
    static_assert(Pack::index_of<int> == std::numeric_limits<size_t>::max());
    static_assert(Pack::index_of<long> == std::numeric_limits<size_t>::max());
    static_assert(Pack::index_of_any<int, long> == std::numeric_limits<size_t>::max());
    static_assert(Pack::index_of_any<> == std::numeric_limits<size_t>::max());
}

TEST(TypePackTests, IndexOf1)
{
    using Pack = TypePack<int>;
    static_assert(Pack::index_of<int> == 0);
    static_assert(Pack::index_of<long> == std::numeric_limits<size_t>::max());
    static_assert(Pack::index_of_any<int, long> == 0);
    static_assert(Pack::index_of_any<char, long> == std::numeric_limits<size_t>::max());
    static_assert(Pack::index_of_any<> == std::numeric_limits<size_t>::max());
}

TEST(TypePackTests, IndexOf)
{
    using Pack = TypePack<int, long, double, char>;
    static_assert(Pack::index_of<int> == 0);
    static_assert(Pack::index_of<long> == 1);
    static_assert(Pack::index_of<double> == 2);
    static_assert(Pack::index_of<char> == 3);
    static_assert(Pack::index_of<char8_t> == std::numeric_limits<size_t>::max());
    static_assert(Pack::index_of_any<char> == 3);
    static_assert(Pack::index_of_any<char, double> == 2);
    static_assert(Pack::index_of_any<unsigned, char8_t> == std::numeric_limits<size_t>::max());
    static_assert(Pack::index_of_any<> == std::numeric_limits<size_t>::max());
}

TEST(TypePackTests, Unique0)
{
    using Pack = TypePack<>;
    static_assert(std::is_same_v<type_pack_unique_t<Pack>, Pack>);
}

TEST(TypePackTests, Unique1)
{
    using Pack = TypePack<int>;
    static_assert(std::is_same_v<type_pack_unique_t<Pack>, TypePack<int>>);
}

TEST(TypePackTests, Unique)
{
    using Pack = TypePack<int, long, int, char, long, long, char, char8_t>;
    static_assert(std::is_same_v<type_pack_unique_t<Pack>, TypePack<int, long, char, char8_t>>);
}

TEST(TypePackTests, ApplyTo)
{
    using Pack = TypePack<int, long, char, double>;
    static_assert(std::is_same_v<Pack::apply_to_t<std::tuple>, std::tuple<int, long, char, double>>);
}
