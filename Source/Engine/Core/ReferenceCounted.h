#pragma once

#include <memory>

namespace bl {

class ReferenceCounted
{
    mutable std::atomic<uint32_t> _count{0};

public:
    ReferenceCounted() = default;
    ReferenceCounted(const ReferenceCounted&) = delete;
    ~ReferenceCounted() = default;

    ReferenceCounted& operator=(const ReferenceCounted&) = delete;

    void AddReference();
    void RemoveReference();
    uint32_t GetReferenceCount();
};

} // namespace bl