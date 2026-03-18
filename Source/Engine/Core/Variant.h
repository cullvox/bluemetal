#pragma once

#include <variant>

namespace bl {

class Object;

using Variant = std::variant<Object*, int64_t, float, double, glm::vec2, glm::vec3, glm::vec4, glm::quat, glm::mat4>;

template<typename VariantType, typename T, std::size_t index = 0>
constexpr std::size_t variant_index() {
    static_assert(std::variant_size_v<VariantType> > index, "Type not found in variant");
    if constexpr (index == std::variant_size_v<VariantType>) {
        return index;
    } else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
        return index;
    } else {
        return variant_index<VariantType, T, index + 1>();
    }
}

enum class VariantType
{
    eObject = variant_index<Variant, Object*>(),
    eInteger = variant_index<Variant, int64_t>(),
    eFloat = variant_index<Variant, float>(),
    eDouble = variant_index<Variant, double>(),
    eVector2 = variant_index<Variant, glm::vec2>(),
    eVector3 = variant_index<Variant, glm::vec3>(),
    eVector4 = variant_index<Variant, glm::vec4>(),
    eQuaternion = variant_index<Variant, glm::quat>(),
    eMatrix4 = variant_index<Variant, glm::mat4>(),
};

template<typename T>
constexpr VariantType GetVariantType()
{
    return static_cast<VariantType>(variant_index<Variant, T>());
}

}