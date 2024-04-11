#pragma once


#define _BL_STRINGIFY(x) #x 
#define BL_STRINGIFY(x) _BL_STRINGIFY(x)

#define BL_HASH_COMBINE(seed, value) \
    (seed ^= std::hash<decltype(value)>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2))