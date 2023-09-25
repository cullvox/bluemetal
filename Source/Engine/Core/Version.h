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

constexpr std::string to_string(VersionRelease release)
{
    switch(release) {
    case VersionRelease::eAlpha: return "Alpha";
    case VersionRelease::eBeta: return "Beta";
    case VersionRelease::eInfiniteDevelopment: return "InfiniteDevelopment";
    case VersionRelease::eRelease: return "Release";
    case VersionRelease::eStaging: return "Staging";
    case VersionRelease::eSnapshot: return "Snapshot";
    default: 
    case VersionRelease::eUndefined: return "Undefined";
    }
}

struct BLUEMETAL_API Version {
    VersionRelease release;
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

std::string to_string(Version version)
{
    return fmt::format("{} {}.{}.{}", bl::to_string(version.release), version.major, version.minor, version.patch);
}

const Version engineVersion(VersionRelease::eAlpha, 0, 1, 0);
const std::string engineName = "Bluemetal Engine";

} // namespace bl

template <>
struct fmt::formatter<bl::VersionRelease>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    format_context::iterator format(bl::VersionRelease release, format_context& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", bl::to_string(release));
    }
};

template <>
struct fmt::formatter<bl::Version>
{
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    format_context::iterator format(const bl::Version& v, format_context& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", to_string(v));
    }
};


