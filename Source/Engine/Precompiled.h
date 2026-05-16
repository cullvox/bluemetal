#pragma once

// StdC
#include <cstddef>
#include <cstring>
#include <cstdint>

// StdCpp
#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string_view>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include <vector>

// Vulkan
#define VK_ENABLE_BETA_EXTENSIONS
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <volk.h>

// SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>



#include <spirv_reflect.h>

#ifdef BLUEMETAL_COMPILER_APPLE_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullable-to-nonnull-conversion"
#endif

// #define VMA_DEBUG_LOG_FORMAT(...) printf(__VA_ARGS__); putc('\n', stdout)

#include <vk_mem_alloc.h>

#ifdef BLUEMETAL_COMPILER_APPLE_CLANG
#pragma clang diagnostic pop
#endif

// GLM
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/ext/vector_int2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

// JSON
#include <nlohmann/json.hpp>

// Fast Noise
#include <FastNoise/FastNoise.h>
