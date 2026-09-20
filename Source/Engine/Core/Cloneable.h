#pragma once

namespace bl {

template<class T>
concept Cloneable = requires(const T& object) {
    {
        object.Clone()
    } -> std::convertible_to<T*>;
};

}