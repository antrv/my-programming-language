#pragma once

#include <algorithm>
#include <array>
#include <string_view>

namespace skarn {

class TypeName
{
    template <class T> // Template parameter name T must be present for Clang.
    static consteval std::string_view raw_typename() noexcept
    {
#ifdef _MSC_VER
        return __FUNCSIG__;
#else
        return __PRETTY_FUNCTION__;
#endif
    }

    template <class T>
    static consteval auto get_typename() noexcept
    {
        struct TypeNameFormat
        {
            std::size_t leadingJunk = 0;
            std::size_t junkTotal = 0;
        };

        constexpr std::string_view str = raw_typename<int>();
        constexpr TypeNameFormat format {str.find("int"), str.size() - 3};
        static_assert(format.leadingJunk != std::string_view::npos,
                      "Unable to determine the type name format on this compiler.");

        constexpr std::string_view rawName = raw_typename<T>();
        constexpr std::string_view name = rawName.substr(format.leadingJunk, rawName.size() - format.junkTotal);
        constexpr std::string_view classPrefix {"class "};
        constexpr std::string_view structPrefix {"struct "};
        constexpr std::string_view enumPrefix {"enum "};
        constexpr std::string_view name1 = name.starts_with(classPrefix) ? name.substr(classPrefix.size()) : name;
        constexpr std::string_view name2 = name1.starts_with(structPrefix) ? name1.substr(structPrefix.size()) : name1;
        constexpr std::string_view name3 = name2.starts_with(enumPrefix) ? name2.substr(enumPrefix.size()) : name2;

        std::array<char, name3.size()> arr {};
        std::copy_n(name3.data(), name3.size(), arr.data());
        return arr;
    }

    template <class T>
    static constexpr auto typeNameHolder = get_typename<T>();

public:
    template <class T>
    static constexpr std::string_view get() noexcept
    {
        return {typeNameHolder<T>.data(), typeNameHolder<T>.size()};
    }

    template <class T>
    static constexpr std::string_view get_unqualified() noexcept
    {
        constexpr std::string_view typeName = get<T>();
        constexpr size_t pos = typeName.rfind("::");
        return pos != std::string_view::npos ? typeName.substr(pos + 2) : typeName;
    }
};

} // namespace skarn
