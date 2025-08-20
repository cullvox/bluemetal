#pragma once

#include "Precompiled.h"
#include "Math/Math.h"
#include "ReferenceCounted.h"

namespace bl {

class Object;

using Variant = std::variant<
    bool,
    int,
    float,
    double,
    glm::vec2,
    glm::ivec2,
    glm::vec3,
    glm::ivec3,
    glm::vec4,
    glm::ivec4,
    glm::quat,
    glm::mat3,
    glm::mat4,
    Ref<Object>>;

template<typename T>
concept VariantType = requires(T) {
    std::same_as<T, bool> ||
    std::same_as<T, int> ||
    std::same_as<T, float> ||
    std::same_as<T, double> ||
    std::same_as<T, glm::vec2> ||
    std::same_as<T, glm::ivec2> ||
    std::same_as<T, glm::vec3> ||
    std::same_as<T, glm::ivec3> ||
    std::same_as<T, glm::vec4> ||
    std::same_as<T, glm::ivec4> ||
    std::same_as<T, glm::quat> ||
    std::same_as<T, glm::mat3> ||
    std::same_as<T, glm::mat4> ||
    std::same_as<T, Ref<Object>>;
};

}