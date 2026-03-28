#pragma once

#include <variant>

namespace bl {

class Object;

using Variant = std::variant<Object*, int64_t, float, double, glm::vec2, glm::vec3, glm::vec4, glm::quat, glm::mat4, std::string>;

template<typename VariantType, typename T, std::size_t index = 0>
constexpr std::size_t VariantTypeIndex() {
    static_assert(std::variant_size_v<VariantType> > index, "Type not found in variant");
    if constexpr (index == std::variant_size_v<VariantType>) {
        return index;
    } else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
        return index;
    } else {
        return VariantTypeIndex<VariantType, T, index + 1>();
    }
}

enum class VariantType
{
    eObject =       VariantTypeIndex<Variant, Object*>(),
    eInteger =      VariantTypeIndex<Variant, int64_t>(),
    eFloat =        VariantTypeIndex<Variant, float>(),
    eDouble =       VariantTypeIndex<Variant, double>(),
    eVector2 =      VariantTypeIndex<Variant, glm::vec2>(),
    eVector3 =      VariantTypeIndex<Variant, glm::vec3>(),
    eVector4 =      VariantTypeIndex<Variant, glm::vec4>(),
    eQuaternion =   VariantTypeIndex<Variant, glm::quat>(),
    eMatrix4 =      VariantTypeIndex<Variant, glm::mat4>(),
    eString =       VariantTypeIndex<Variant, std::string>(),
};

template<typename T>
constexpr VariantType GetVariantType()
{
    return static_cast<VariantType>(VariantTypeIndex<Variant, T>());
}

}