#pragma once

#include "ReferenceCounted.h"

namespace bl
{

template<typename T>
class Reference
{
    ReferenceCounted* _object;

public:
    Reference()
        : _object(nullptr)
    {
    }

    Reference(const Reference& other)
        : _object(other._object)
    {
        if (_object)
            _object->AddReference();
    }

    Reference(Reference&& other)
        : _object(other._object)
    {
        other._object = nullptr;
    }

    Reference(T* counted)
    {
        counted->AddReference();
    }

    ~Reference()
    {
        if (_object)
            _object->RemoveReference();
    }

    Reference& operator=(const Reference& other) noexcept 
    {
        if (this != &other)
        {
            if (other._object)
                other._object->AddReference();
            if (_object)
                _object->RemoveReference();
            _object = other._object;
        }

        return *this;
    }

    Reference& operator=(Reference&& other) noexcept
    {
        if (this == &other)
            return;

        if (_object)
            _object->RemoveReference();
        _object = other._object;
        other._object = nullptr;
        return *this;
    }

    T* operator->()
    {
        return _object;
    }

    bool Valid() const;
};

template<typename T>
using Ref = Reference<T>;

template<typename T, typename...TArgs>
Ref<T> MakeRef(TArgs&&...args)
{
    return Ref<T>(new T(std::forward<TArgs>(args)...));
}

}