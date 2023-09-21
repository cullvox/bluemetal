#pragma once

#include "Export.h"
#include "Precompiled.h"

namespace bl
{

enum class VersionRelease : uint32_t
{
    eAlpha,
    eBeta,
    eInfiniteDevelopment,
    eRelease,
    eStaging,
    eSnapshot,
    eUndefined,
};

struct BLUEMETAL_API Version
{
    VersionRelease release;
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

const Version engineVersion(VersionRelease::eAlpha, 0, 1, 0);
const std::string engineName = "Bluemetal Engine";

} // namespace bl

template <>
struct fmt::formatter<bl::VersionRelease>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    format_context::iterator format(bl::VersionRelease v, format_context& ctx) const
    {
        std::string_view val;
        switch(v) {
        case bl::VersionRelease::eAlpha: val = "Alpha";
        case bl::VersionRelease::eBeta: val ="Beta";
        case bl::VersionRelease::eInfiniteDevelopment: val = "InfiniteDevelopment";
        case bl::VersionRelease::eRelease: val = "Release";
        case bl::VersionRelease::eStaging: val = "Staging";
        case bl::VersionRelease::eSnapshot: val = "Snapshot";
        case bl::VersionRelease::eUndefined:
        default:
            val = "Undefined";
        return fmt::format_to(ctx.out(), "{}", val);
    }
  }
};

template <>
struct fmt::formatter<bl::Version>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    format_context::iterator format(const bl::Version& v, format_context& ctx) const
    {
        return fmt::format_to(ctx.out(), "{} {}.{}.{}", v.release, v.major, v.minor, v.patch);
    }
};


