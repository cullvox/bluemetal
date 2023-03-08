#pragma once

#include "Core/Export.h"

#include <source_location>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>

namespace bl 
{

class BLOODLUST_CORE_API Logger
{
public:

    struct format_string {
        fmt::string_view str;
        std::source_location loc;

        format_string(const char* str, const std::source_location& loc = std::source_location::current()) 
            : str(str), loc(loc) 
        {
        }
    };

    template<typename... Args>
    static void Log(const format_string& format, Args&&... args)
    {
        vlog(format, fmt::make_format_args(args...));
    }

    template<typename... Args>
    static void Error(const format_string& format, Args&&... args)
    {
        velog(format, fmt::make_format_args(args...));
    }


    template<typename... Args>
    static void Debug(const format_string& format, Args&&... args)
    {
#ifndef NDEBUG
        vdlog(format, fmt::make_format_args(args...));
#endif
    }


private:
    static void vdlog(const format_string& format, fmt::format_args args) {
        const auto& loc = format.loc;
        const std::string path = loc.file_name();
        const std::string filename = path.substr(path.find_last_of("/\\") + 1);

        spdlog::log(spdlog::level::debug, "{}:{}: ", filename, loc.line());
        fmt::vprint(format.str, args);
    }

    static void vlog(const format_string& format, fmt::format_args args) {
        const auto& loc = format.loc;
        const std::string path = loc.file_name();
        const std::string filename = path.substr(path.find_last_of("/\\") + 1);

        spdlog::log(spdlog::level::info, "{}:{}: ", filename, loc.line());
        fmt::vprint(format.str, args);
    }

    static void velog(const format_string& format, fmt::format_args args) {
        const auto& loc = format.loc;
        const std::string path = loc.file_name();
        const std::string filename = path.substr(path.find_last_of("/\\") + 1);

        spdlog::log(spdlog::level::err, "{}:{}: ", filename, loc.line());
        fmt::vprint(format.str, args);
    }
};

} // namespace bl