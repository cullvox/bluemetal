#pragma once

#include "Precompiled.h"

namespace bl {

class Object;
class ObjectClass;

template <typename T>
concept ObjectType = std::is_convertible_v<T, Object> && (std::is_default_constructible_v<T> || std::is_abstract_v<T>) && requires(T) {
    { T::_Creator() } -> std::same_as<Object*>;

    { T::BindProperties() } -> std::same_as<void>;
    { T::ClassName() } -> std::same_as<std::string_view>;
    { T::ParentClassName() } -> std::same_as<std::string_view>;
    { T::GetObjectClass() } -> std::same_as<const ObjectClass&>;
};

}