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
        using duration = std::chrono::duration<float, std::milli>;

        auto now = std::chrono::high_resolution_clock::now();

        duration dt = now - start;
        return dt.count();
    }

private:
    static inline const std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
};

} // namespace bl