#pragma once

namespace bl {

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
