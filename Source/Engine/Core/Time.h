#pragma once

///////////////////////////////
// Headers
///////////////////////////////

#include "Precompiled.h"

namespace bl
{

///////////////////////////////
// Classes
///////////////////////////////

class Time
{
public:
    static float current()
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> dt = now - start;
        return dt.count();
    }

private:
    static inline const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
};

} // namespace bl