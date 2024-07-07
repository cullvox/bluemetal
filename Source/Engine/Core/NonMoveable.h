#pragma once

namespace bl {

/// @brief Any object that should not be moved.
class NonMoveable {
public:
    NonMoveable() = default;
    NonMoveable(NonMoveable&) = default;
    NonMoveable(NonMoveable&&) = delete;
    ~NonMoveable() = default;

    NonMoveable& operator=(NonMoveable&) = default;
    NonMoveable& operator=(NonMoveable&&) = delete;
};

} // namespace bl