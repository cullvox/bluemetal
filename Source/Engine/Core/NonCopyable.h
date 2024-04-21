#pragma once

namespace bl
{

class NonCopyable /** @brief Any object that should not be copied. */
{
public:
    NonCopyable() = default;
    NonCopyable(NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;
    ~NonCopyable() = default;

    NonCopyable& operator=(NonCopyable&) = delete;
    NonCopyable& operator=(NonCopyable&&) = default;
};

} // namespace bl