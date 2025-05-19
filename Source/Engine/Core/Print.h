#pragma once

#include <cstdlib>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/format.h>

#include "PathUtils.h"

namespace bl 
{

namespace logging 
{
    void enableVerboseLogging(bool enable);
    bool isVerboseLogging();
}

extern bool useVerboseLogging;

#define blDebug(FORMAT, ...) \
    fmt::print(fg(fmt::color::orange), "DEBUG {} {} {} : {}\n", bl::PathUtils::GetFilename(__FILE__), __LINE__, __func__, fmt::format(FMT_COMPILE(FORMAT) __VA_OPT__(,) __VA_ARGS__))

#define blVerbose(FORMAT, ...) \
    if (bl::logging::isVerboseLogging()) \
        fmt::print(fg(fmt::color::blanched_almond), "VERBOSE {} {} {} : {}\n", bl::PathUtils::GetFilename(__FILE__), __LINE__, __func__, fmt::format(FMT_COMPILE(FORMAT) __VA_OPT__(,) __VA_ARGS__))

#define blInfo(FORMAT, ...) \
    fmt::print(fg(fmt::color::white), "INFO {} {} {} : {}\n", bl::PathUtils::GetFilename(__FILE__), __LINE__, __func__, fmt::format(FMT_COMPILE(FORMAT) __VA_OPT__(,) __VA_ARGS__))

#define blWarning(FORMAT, ...) \
    fmt::print(fg(fmt::color::yellow), "WARNING {} {} {} : {}\n", bl::PathUtils::GetFilename(__FILE__), __LINE__, __func__, fmt::format(FMT_COMPILE(FORMAT) __VA_OPT__(,) __VA_ARGS__))

#define blError(FORMAT, ...) \
    fmt::print(bg(fmt::color::red), "ERROR {} {} {} : {}\n", bl::PathUtils::GetFilename(__FILE__), __LINE__, __func__, fmt::format(FMT_COMPILE(FORMAT) __VA_OPT__(,) __VA_ARGS__))

} // namespace bl
