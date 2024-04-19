#pragma once

namespace bl {

template<typename TValue>
size_t hash_combine(size_t seed, TValue value) {
    return seed ^ std::hash<TValue>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

} // namespace bl