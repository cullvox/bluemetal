#pragma once

#include "ReferenceCounted.h"

#include <type_traits>
#include <utility>

namespace bl
{

template<typename T, typename TOther>
concept ReferenceUpcastable = std::is_convertible_v<TOther*, T*>;

template<typename T>
class Reference
{
    template<typename>
    friend class Reference;

    ReferenceCounted* _object;

public:
    using ValueType = T;

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
        : _object(counted)
    {
        if (_object)
            _object->AddReference();
    }

    template<typename TOther>
    requires ReferenceUpcastable<T, TOther>
    Reference(const Reference<TOther>& other)
        : _object(static_cast<T*>(other._object))
    {
        if (_object)
            _object->AddReference();
    }

    template<typename TOther>
    requires ReferenceUpcastable<T, TOther>
    Reference(Reference<TOther>&& other) noexcept
        : _object(other._object)
    {
        other._object = nullptr;
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
            return *this;

        if (_object)
            _object->RemoveReference();
        _object = other._object;
        other._object = nullptr;
        return *this;
    }

    template<typename TOther>
    requires ReferenceUpcastable<T, TOther>
    Reference& operator=(const Reference<TOther>& other) noexcept
    {
        if (_object == other._object)
            return *this;

        if (other._object)
            other._object->AddReference();
        if (_object)
            _object->RemoveReference();
        _object = other._object;
        return *this;
    }

    template<typename TOther>
    requires ReferenceUpcastable<T, TOther>
    Reference& operator=(Reference<TOther>&& other) noexcept
    {
        if (_object == other._object)
            return *this;

        if (_object)
            _object->RemoveReference();
        _object = other._object;
        other._object = nullptr;
        return *this;
    }

    T* operator->()
    {
        return static_cast<T*>(_object);
    }

    const T* operator->() const
    {
        return static_cast<const T*>(_object);
    }

    T& operator*()
    {
        return *(static_cast<T*>(_object));
    }

    const T& operator*() const
    {
        return *(static_cast<const T*>(_object));
    }

    template<typename TCast>
    Reference<TCast> Cast()
    {
        return Reference<TCast>(dynamic_cast<TCast*>(_object));
    }

    operator bool() const noexcept { 
        return _object != nullptr;
    }

    bool operator==(const Reference<T>& other) const {
        return _object == other._object;
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