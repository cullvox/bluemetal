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
        auto now = std::chrono::high_resolution_clock::now();

        fmt::print(fmt::emphasis::bold | fg(fmt::color::dark_orange), "DEBUG | {:%Y-%m-%d-%H:%M:%S} | {}:{}():{} - ", fmt::localtime(std::chrono::system_clock::now()), filename, loc.function_name(), loc.line());
        fmt::vprint(format.str, args);
        fmt::print("\n");
    }

    static void vlog(const format_string& format, fmt::format_args args) {
        const auto& loc = format.loc;
        const std::string path = loc.file_name();
        const std::string filename = path.substr(path.find_last_of("/\\") + 1);
        auto now = std::chrono::high_resolution_clock::now();

        fmt::print(fg(fmt::color::antique_white), "INFO | {:%Y-%m-%d %H:%M:%S} | {}:{}():{} - ", fmt::localtime(std::chrono::system_clock::now()), filename, loc.function_name(), loc.line());
        fmt::vprint(format.str, args);
        fmt::print("\n");
    }

    static void velog(const format_string& format, fmt::format_args args) {
        const auto& loc = format.loc;
        const std::string path = loc.file_name();
        const std::string filename = path.substr(path.find_last_of("/\\") + 1);
        auto now = std::chrono::high_resolution_clock::now();

        fmt::print(fmt::emphasis::bold | fg(fmt::color::dark_red), "ERROR | {:%Y-%m-%d-%H:%M:%S} | {}:{}():{} - ", fmt::localtime(std::chrono::system_clock::now()), filename, loc.function_name(), loc.line());
        fmt::vprint(format.str, args);
        fmt::print("\n");
    }
};

} // namespace bl