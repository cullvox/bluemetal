#pragma once

#include <cstdlib>
#include <fmt/core.h>

#include "PathUtils.h"

namespace bl {

#define blDebug(FORMAT, ...) \
    fmt::print(fg(fmt::color::orange), "DEBUG {} {} {} : {}\n", PathUtils::GetFilename(__FILE__), __LINE__, __func__, fmt::format(FMT_COMPILE(FORMAT) __VA_OPT__(,) __VA_ARGS__))

#define blInfo(FORMAT, ...) \
    fmt::print(fg(fmt::color::white), "INFO {} {} {} : {}\n", PathUtils::GetFilename(__FILE__), __LINE__, __func__, fmt::format(FMT_COMPILE(FORMAT) __VA_OPT__(,) __VA_ARGS__))

#define blWarning(FORMAT, ...) \
    fmt::print(fg(fmt::color::yellow), "WARNING {} {} {} : {}\n", PathUtils::GetFilename(__FILE__), __LINE__, __func__, fmt::format(FMT_COMPILE(FORMAT) __VA_OPT__(,) __VA_ARGS__))

#define blError(FORMAT, ...) \
    fmt::print(bg(fmt::color::red), "ERROR {} {} {} : {}\n", PathUtils::GetFilename(__FILE__), __LINE__, __func__, fmt::format(FMT_COMPILE(FORMAT) __VA_OPT__(,) __VA_ARGS__))

} // namespace bl
