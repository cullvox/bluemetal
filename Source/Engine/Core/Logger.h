#pragma once

#include <spdlog/spdlog.h>

namespace bl {

class Logger
{
public:
    Logger();
    ~Logger();

    void Info();
    void Error();
};

#define BL_TRACE(...)   spdlog::trace(__VA_ARGS__)
#define BL_INFO(...)    spdlog::info(__VA_ARGS__)
#define BL_WARN(...)    spdlog::warn(__VA_ARGS__)
#define BL_ERROR(...)   spdlog::error(__VA_ARGS__)
#define BL_FATAL(...)   spdlog::critical(__VA_ARGS__)

} // namespace bl