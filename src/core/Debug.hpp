#pragma once

#include <source_location>

#include <fmt/core.h>
    
class Debug
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
#ifndef NDEBUG
        vlog(format, fmt::make_format_args(args...));
#endif
    }

private:
    static void vlog(const format_string& format, fmt::format_args args) {
        const auto& loc = format.loc;
        fmt::print("{}:{}: ", loc.file_name(), loc.line());
        fmt::vprint(format.str, args);
    }
};