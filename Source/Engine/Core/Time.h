#pragma once

#include <chrono>

namespace bl {

class Time {
public:
    static float Current()
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> dt = now - start;
        return dt.count();
    }

private:
    static inline const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
};

} // namespace bl