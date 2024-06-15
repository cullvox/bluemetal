#pragma once

#include "Precompiled.h"
#include "Core/Print.h"
#include <sys/types.h>

namespace bl {

enum class VersionRelease : uint32_t {
    eAlpha,
    eBeta,
    eDev,
    eInfDev,
    eRelease,
    eStaging,
    eSnapshot,
};

static inline constexpr std::string to_string(VersionRelease release) {
    switch(release) {
    case VersionRelease::eAlpha: return "alpha";
    case VersionRelease::eBeta: return "beta";
    case VersionRelease::eDev: return "dev";
    case VersionRelease::eInfDev: return "infdev";
    case VersionRelease::eRelease: return "release";
    case VersionRelease::eStaging: return "staging";
    case VersionRelease::eSnapshot: return "snapshot";
    default: return "undefined";
    }
}

struct Version {
    Version()
        : release(VersionRelease::eAlpha), major(0), minor(1), patch(0) {}

    Version(VersionRelease release, uint32_t major, uint32_t minor, uint32_t patch)
        : release(release), major(major), minor(minor), patch(patch) {}

    VersionRelease release;
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

static inline std::string to_string(Version version) {
    return fmt::format("{} {}.{}.{}", bl::to_string(version.release), version.major, version.minor, version.patch);
}

const Version engineVersion(VersionRelease::eAlpha, 0, 1, 0);
const std::string engineName = "Bluemetal Engine";

} // namespace bl

template <>
struct fmt::formatter<bl::VersionRelease> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    format_context::iterator format(bl::VersionRelease release, format_context& ctx) const {
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


