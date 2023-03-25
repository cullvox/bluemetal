#pragma once

#include "Core/Export.h"

#include <source_location>

#include <fmt/color.h>
#include <fmt/chrono.h>

namespace bl 
{

class BLOODLUST_API Logger
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
        vlog(fg(fmt::color::antique_white), "INFO", format, fmt::make_format_args(args...));
    }

    template<typename... Args>
    static void Warning(const format_string& format, Args&&... args)
    {
        vlog(fmt::emphasis::bold | fg(fmt::color::yellow), "WARNING", format, fmt::make_format_args(args...));
    }

    template<typename... Args>
    static void Error(const format_string& format, Args&&... args)
    {
        vlog(fmt::emphasis::bold | fg(fmt::color::dark_red), "ERROR", format, fmt::make_format_args(args...));
    }

    template<typename... Args>
    static void Debug(const format_string& format, Args&&... args)
    {
#ifndef NDEBUG
        vlog(fmt::emphasis::bold | fg(fmt::color::dark_orange), "DEBUG", format, fmt::make_format_args(args...));
#endif
    }

private:
    static void vlog(const fmt::text_style& style, const std::string_view& type, const format_string& format, fmt::format_args args) {
        const auto& loc = format.loc;
        const std::string path = loc.file_name();
        const std::string filename = path.substr(path.find_last_of("/\\") + 1);
        auto now = std::chrono::high_resolution_clock::now();

        fmt::print(style, "{} | {:%Y-%m-%d-%H:%M:%S} | {}:{}():{} - ", type, fmt::localtime(std::chrono::system_clock::now()), filename, loc.function_name(), loc.line());
        fmt::vprint(fmt::format(style, "{}", format.str), args);
        fmt::print("\n");

        
    }
};

} // namespace bl