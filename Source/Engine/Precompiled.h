#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "Graphics/vk_mem_alloc.h"
#include "spirv_reflect.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_pixels.h>
#include <SDL_vulkan.h>

#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/chrono.h>

#include <cstdint>
#include <cmath>
#include <cassert>

#include <stdexcept>
#include <memory>
#include <chrono>
#include <vector>
#include <array>
#include <string>
#include <string_view>
#include <optional>
#include <functional>
#include <optional>
#include <queue>
#include <list>
#include <unordered_set>
#include <variant>
#include <map>
#include <filesystem>
#include <span>
#include <fstream>