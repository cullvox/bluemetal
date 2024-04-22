#pragma once

namespace bl
{

class NonMoveable /** @brief Any object that should not be copied. */
{
public:
    NonMoveable() = default;
    NonMoveable(NonMoveable&) = default;
    NonMoveable(NonMoveable&&) = delete;
    ~NonMoveable() = default;

    NonMoveable& operator=(NonMoveable&) = default;
    NonMoveable& operator=(NonMoveable&&) = delete;
};

} // namespace bl