#pragma once

#include "Core/NonMoveable.h"

namespace bl
{

class ReferenceCounted /** @brief Any class that needs to know how many times it's being used. */
{
public:
    ReferenceCounted() = default;
    ~ReferenceCounted() = default;

    int GetReferenceCount();

protected:
    template<typename T>
    friend class ReferenceCounter;

    void IncreaseRefs() { _count++; }
    void DecreaseRefs() { _count--; }

private:
    int _count;
};

template<typename T>
class ReferenceCounter : public NonMoveable
{
    static_assert(std::is_base_of_v<ReferenceCounted, T>);
public:
    ReferenceCounter(T& value) 
    { 
        _value->IncreaseRefs(); 
    }
    
    ReferenceCounter(ReferenceCounter& copy) 
    {
        _value->DecreaseRefs();
        _value = copy._value;
        _value->IncreaseRefs();
    }

    ~ReferenceCounter()
    { 
        _value->DecreaseRefs(); 
    }

    ReferenceCounter& operator=(ReferenceCounter& rhs)
    {
        _value->DecreaseRefs();
        _value = copy._value;
        _value->IncreaseRefs(); 
    }

private:
    ReferenceCounted* _value; /** @brief A reference to our counted resource, should never be nullptr. */
};

} // namespace bl