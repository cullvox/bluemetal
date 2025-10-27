#pragma once

#include "Object.h"
#include "Precompiled.h"

namespace bl 
{

class ReferenceBase 
{
    bool _updated = false;
    bool _isValid = false;
    // int32_t _refId = -1;

protected:
    friend class ReferenceCounted;
    virtual void SetUpdated(bool updated) { _updated = updated; }
    virtual void SetValid(bool isValid) { _isValid = isValid; }

    // bool operator==(const ReferenceBase& other) const
    // {
    //     return _refId == other._refId;
    // }

public:
    ReferenceBase() = default;
    ReferenceBase(const ReferenceBase&) = default;
    ReferenceBase& operator=(const ReferenceBase&) = default;
    ReferenceBase(ReferenceBase&&) = default;
    ReferenceBase& operator=(ReferenceBase&&) = default;
    virtual ~ReferenceBase() = default;

    bool IsValid() const { return _isValid; }
    bool WasUpdated() const { return _updated; }; /** @brief This function is called when the reference is updated, usually to notify the resource that it has been used. */
    void UnmarkUpdated() { _updated = false; } /** @brief Unmarks the updated state, usually called after the resource has been processed. */
};

/// @brief Any class that needs to know how many times it's being used.
class ReferenceCounted 
{
    std::list<ReferenceBase*> _references;

protected:
    template <class T>
    friend class ReferenceCounter;

    void UpdateReferences()
    {
        for (auto& ref : _references) {
            ref->SetUpdated(true);
        }
    }

    void InvalidateReferences()
    {
        for (auto& ref : _references) {
            ref->SetUpdated(true);
            ref->SetValid(false);
        }

        _references.clear();
    }

    void AddReference(ReferenceBase& ref)
    {
        _references.push_back(&ref);
        ref.SetValid(true); // Mark as valid
        // ref._refId = _references.size() - 1; // Assign a unique ID to the reference
    }

    void RemoveReference(ReferenceBase& ref)
    {
        _references.remove(&ref);
        ref.SetUpdated(true);
        ref.SetValid(false); // Mark as invalid
        // ref._refId = -1; // Mark as invalid
        // ref._updated = false; // Reset updated state
    }

public:
    ReferenceCounted() = default;
    ~ReferenceCounted()
    {
        for (auto& ref : _references) {
            // ref._refId = -1; // Mark as invalid
            ref->SetValid(false); // Reset updated state
        }
    }

    std::size_t GetReferenceCount()
    {
        return _references.size();
    }
};


template <class T>
class ReferenceCounter : public ReferenceBase 
{
    T* _value;

public:
    ReferenceCounter()
        : _value(nullptr)
    {
    }

    ReferenceCounter(T* value)
        : _value(value)
    {
        if (dynamic_cast<ReferenceCounted*>(value) == nullptr)
            throw std::runtime_error("Reference counter type not based on ReferenceCounted!");

        _value->AddReference(*this);
    }

    ReferenceCounter(const ReferenceCounter& copy)
        : ReferenceBase(copy)
        , _value(copy._value)
    {
        if (IsValid())
            _value->AddReference(*this);
    }

    ~ReferenceCounter()
    {
        if (IsValid())
            _value->RemoveReference(*this);
    }

    ReferenceCounter& operator=(const ReferenceCounter& rhs)
    {
        _value = rhs._value;
        if (_value)
            _value->AddReference(*this);
        return *this;
    }

    T* Get() const
    {
        return IsValid() ? _value : nullptr;
    }

    T* operator->()
    {
        return IsValid() ? _value : nullptr;
    }
};

template <class T>
using Ref = ReferenceCounter<T>; /** @brief Rename the reference counter to something a little more useful. */

} // namespace bl