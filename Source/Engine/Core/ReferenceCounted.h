#pragma once

#include "Core/NonMoveable.h"

namespace bl {

/// @brief Any class that needs to know how many times it's being used.
class ReferenceCounted {
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
class ReferenceCounter : public NonMoveable {
    static_assert(std::is_base_of_v<ReferenceCounted, T>);
public:
    ReferenceCounter()
    {
    }

    ReferenceCounter(T* value)
        : _value(value)
    { 
        _value->IncreaseRefs(); 
    }
    
    ReferenceCounter(const ReferenceCounter& copy)
        : _value(copy._value)
    {
        _value = copy._value;

        if (_value)
            _value->IncreaseRefs();
    }

    ~ReferenceCounter()
    { 
        if (_value)
            _value->DecreaseRefs(); 
    }

    ReferenceCounter& operator=(const ReferenceCounter& rhs)
    {
        _value = rhs._value;
        if (_value)
            _value->IncreaseRefs();
        return *this;
    }

    T* Get() const
    {
        return _value;
    }

private:
    T* _value; /** @brief A reference to our counted resource, should never be nullptr. */
};

} // namespace bl