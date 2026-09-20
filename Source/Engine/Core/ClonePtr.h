#pragma once

#include "Cloneable.h"

namespace bl {

template<typename T, typename TOther>
concept CReferenceUpcastable = std::is_convertible_v<TOther*, T*>;

// A deeply copyable pointer.
template<Cloneable T>
class ClonePtr {

    template<Cloneable>
    friend class ClonePtr;

    T* _ptr;

public:
    ClonePtr() noexcept
        : _ptr(nullptr) {}
    ClonePtr(T* ptr) noexcept
        : _ptr(ptr) {}
    explicit ClonePtr(const ClonePtr& other)
    {
        _ptr = other._ptr->Clone();
    }

    ClonePtr(ClonePtr&& other) noexcept
    {
        _ptr = other._ptr;
        other._ptr = nullptr; 
    }

    template<typename TOther>
    requires CReferenceUpcastable<T, TOther>
    ClonePtr(ClonePtr<TOther>&& other) noexcept
    {
        _ptr = static_cast<T*>(other._ptr);
        other._ptr = nullptr;
    }

    ~ClonePtr()
    {
        if (_ptr)
            delete _ptr;
    }

    ClonePtr& operator=(const ClonePtr& other)
    {
        _ptr = other._ptr->Clone();
    }

    ClonePtr& operator=(ClonePtr&& other) noexcept
    {
        _ptr = other._ptr;
        other._ptr = nullptr;
    }

    T* operator->()
    {
        return _ptr;
    }

    const T* operator->() const
    {
        return _ptr;
    }

    T& operator*()
    {
        return *_ptr;
    }

    const T& operator*() const
    {
        return *_ptr;
    }

    T* Get()
    {
        return _ptr;
    }

    const T* Get() const
    {
        return _ptr;
    }

    T* Release()
    {
        _ptr = nullptr;
        return _ptr;
    }

    template<typename TOther>
    ClonePtr<TOther> operator=(ClonePtr<T>&& other)
    {
        static_assert(std::is_convertible_v<T*, TOther*>, "Must be convertable!");

        TOther* casted = dynamic_cast<TOther*>(other._ptr);
        if (!casted) {
            throw std::runtime_error("Could not cast a clone ptr!");
        }

        _ptr = casted;
        other._ptr = nullptr;
    }

};

template<typename T, typename...TArgs>
ClonePtr<T> MakeClone(TArgs&&...args)
{
    return ClonePtr<T>(new T(std::forward<TArgs>(args)...));
}

}