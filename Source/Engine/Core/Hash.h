#pragma once

#include "Precompiled.h"

namespace bl 
{

#define BL_HASH_DEFAULT_SEED 0x59CFA54A2CD1

// Primary template for hash_combine function
template <typename T>
void hash_combine(std::size_t& seed, const T& value) {
    seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

// Recursive variadic template for hash_combine function
template <typename T, typename... Rest>
void hash_combine(std::size_t& seed, const T& first, const Rest&... rest) {
    hash_combine(seed, first);
    hash_combine(seed, rest...);
}

} // namespace bl