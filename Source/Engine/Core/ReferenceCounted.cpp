#include "ReferenceCounted.h"


namespace bl {

void ReferenceCounted::AddReference()
{
    _count.fetch_add(1, std::memory_order_relaxed);
}

void ReferenceCounted::RemoveReference()
{
    if (_count.fetch_sub(1, std::memory_order_relaxed) == 1) {
        delete this;
    }
}

uint32_t ReferenceCounted::GetReferenceCount()
{
    return _count.load(std::memory_order_relaxed);
}

}