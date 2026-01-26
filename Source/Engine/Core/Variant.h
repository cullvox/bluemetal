#pragma once

#include <variant>

namespace bl {

using Variant = std::variant<uint8_t, uint16_t, uint32_t, uint64_t, int8_t, int16_t, int32_t, int64_t, float, double, glm::vec2, glm::vec3, glm::vec4, glm::quat, glm::mat4>;

}