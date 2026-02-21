#pragma once

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/std.h>

#include "PathUtils.h"

namespace bl {

struct FormatWithLocation {
    std::string_view value;
    std::source_location location;

    template <typename TString>
    FormatWithLocation(const TString& s, const std::source_location& location = std::source_location::current())
        : value(s)
        , location(location)
    {
    }
};

class Print {
    static bool useVerboseLogging;

public:
    static void EnableVerboseLogging(bool enable);
    static bool IsVerboseLogging();

    static void VLog(const FormatWithLocation& format, const fmt::text_style& style, fmt::format_args args)
    {
        const auto& loc = format.location;
        fmt::print(style, "{}:{}: ", bl::PathUtils::GetFilename(loc.file_name()), loc.line());
        fmt::vprint(stdout, style, format.value, args);
        fmt::print("\n");
    }

    static void NewLine()
    {
        fmt::print("\n");
    }

    template <typename... TArgs>
    static void Verbose(FormatWithLocation fmt, TArgs&&... args)
    {
        if (IsVerboseLogging())
            VLog(fmt, fg(fmt::color::cyan), fmt::make_format_args(args...));
    }

    template <typename... TArgs>
    static void Raw(const std::string_view& fmt, TArgs&&... args)
    {
        fmt::vprint(fmt, fmt::make_format_args(args...));
    }

    template <typename... TArgs>
    static void Raw(const fmt::text_style& style, const std::string_view& fmt, TArgs&&... args)
    {
        fmt::vprint(stdout, style, fmt, fmt::make_format_args(args...));
    }


    template <typename... TArgs>
    static void Debug(FormatWithLocation fmt, TArgs&&... args)
    {
        VLog(fmt, fg(fmt::color::orange), fmt::make_format_args(args...));
    }

    template <typename... TArgs>
    static void Info(FormatWithLocation fmt, TArgs&&... args)
    {
        VLog(fmt, fg(fmt::color::white), fmt::make_format_args(args...));
    }

    template <typename... TArgs>
    static void Warn(FormatWithLocation fmt, TArgs&&... args)
    {
        VLog(fmt, fg(fmt::color::yellow), fmt::make_format_args(args...));
    }

    template <typename... TArgs>
    static void Error(FormatWithLocation fmt, TArgs&&... args)
    {
        VLog(fmt, fg(fmt::color::red), fmt::make_format_args(args...));
    }
};

} // namespace bl
