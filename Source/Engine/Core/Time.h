#pragma once

#include <chrono>

namespace bl {

class Time {
public:
    static float Current()
    {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::deci> dt = now - start;
        return dt.count();
    }

private:
    static inline const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
};

} // namespace bl