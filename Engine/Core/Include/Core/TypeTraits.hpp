#pragma once

#include <cinttypes>
#include <type_traits>

// Type Lists
template<typename... TType>
struct TypeList
{
    using Type = TypeList;
    static constexpr auto size = sizeof...(TType); 
};

template<std::size_t, typename>
struct TypeListElement;

template<std::size_t TIndex, typename TFirst, typename... TOther>
struct TypeListElement<TIndex, TypeList<TFirst, TOther...>>
    : TypeListElement<TIndex - 1u, TypeList<TOther...>> {};

template<typename TFirst, typename... TOther>
struct TypeListElement<0u, TypeList<TFirst, TOther...>>
{
    using Type = TFirst;
};

template<std::size_t TIndex, typename TList>
using TypeListElement_t = typename TypeListElement<TIndex, TList>::Type;


// Constness
template<typename TTo, typename TFrom>
struct ConstnessAs
{
    using Type = std::remove_const_t<TTo>;
};

template<typename TTo, typename TFrom>
struct ConstnessAs<TTo, const TFrom>
{
    using Type = const TTo;
};

template<typename TTo, typename TFrom>
using ConstnessAs_t = typename ConstnessAs<TTo, TFrom>::Type;