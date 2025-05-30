#pragma once

#include <cstdlib>

#include <fmt/std.h>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/format.h>
#include <source_location>

#include "PathUtils.h"

namespace bl 
{

class Log
{
public:
    struct FormatWithLocation 
    {
        std::string_view value;
        std::source_location location;

        template <typename TString>
        FormatWithLocation(const TString &s, const std::source_location &location = std::source_location::current())
            : value{s}, location{location} 
        {
        }
    };

    static void EnableVerboseLogging(bool enable);
    static bool IsVerboseLogging();

    static void VLog(const FormatWithLocation& format, const fmt::text_style& style, fmt::format_args args) 
    {
        const auto& loc = format.location;
        fmt::print(style, "{}:{}: ", bl::PathUtils::GetFilename(loc.file_name()), loc.line());
        fmt::vprint(stdout, style,format.value, args);
    }

    template<typename...TArgs>
    static void Verbose(FormatWithLocation fmt, TArgs&&... args)
    {
        if (IsVerboseLogging())
            VLog(fmt, fg(fmt::color::cyan), fmt::make_format_args(args...));
    }


    template<typename...TArgs>
    static void Debug(FormatWithLocation fmt, TArgs&&... args)
    {
        VLog(fmt, fg(fmt::color::orange), fmt::make_format_args(args...));
    }

    template<typename...TArgs>
    static void Info(FormatWithLocation fmt, TArgs&&... args)
    {
        VLog(fmt, fg(fmt::color::white), fmt::make_format_args(args...));
    }

    template<typename...TArgs>
    static void Warn(FormatWithLocation fmt, TArgs&&... args)
    {
        VLog(fmt, fg(fmt::color::yellow), fmt::make_format_args(args...));
    }

    template<typename...TArgs>
    static void Error(FormatWithLocation fmt, TArgs&&... args)
    {
        VLog(fmt, fg(fmt::color::red), fmt::make_format_args(args...));
    }

private:
    static bool useVerboseLogging;
};

} // namespace bl
