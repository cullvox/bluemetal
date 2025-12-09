#pragma once

#include <bitset>
#include <type_traits>

namespace bl {

constexpr int bit(int index)
{
    return 1 << index;
}

template <typename E>
using TFlags = std::bitset<E::Count>;

template <typename E>
using Flags = std::underlying_type_t<E>;

#define BL_DEFINE_FLAG_OPS(ENUM)                                                                                                   \
    inline ENUM operator|(ENUM a, ENUM b) { return static_cast<ENUM>(static_cast<Flags<ENUM>>(a) | static_cast<Flags<ENUM>>(b)); } \
    inline ENUM operator&(ENUM a, ENUM b) { return static_cast<ENUM>(static_cast<Flags<ENUM>>(a) & static_cast<Flags<ENUM>>(b)); } \
    inline ENUM operator^(ENUM a, ENUM b) { return static_cast<ENUM>(static_cast<Flags<ENUM>>(a) ^ static_cast<Flags<ENUM>>(b)); } \
    inline ENUM operator~(ENUM a) { return static_cast<ENUM>(~static_cast<Flags<ENUM>>(a)); }                                      \
    inline ENUM& operator|=(ENUM& a, ENUM b)                                                                                       \
    {                                                                                                                              \
        a = a | b;                                                                                                                 \
        return a;                                                                                                                  \
    }                                                                                                                              \
    inline ENUM& operator&=(ENUM& a, ENUM b)                                                                                       \
    {                                                                                                                              \
        a = a & b;                                                                                                                 \
        return a;                                                                                                                  \
    }                                                                                                                              \
    inline ENUM& operator^=(ENUM& a, ENUM b)                                                                                       \
    {                                                                                                                              \
        a = a ^ b;                                                                                                                 \
        return a;                                                                                                                  \
    }

} // namespace bl
