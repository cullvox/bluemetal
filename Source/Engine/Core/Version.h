#pragma once

#include <string>
#include <string_view>
#include <fmt/format.h>

#include "Core/Print.h"

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

static inline constexpr std::string_view to_string(VersionRelease release)
{
    switch (release) {
    case VersionRelease::eAlpha:
        return "Alpha";
    case VersionRelease::eBeta:
        return "Beta";
    case VersionRelease::eDev:
        return "Dev";
    case VersionRelease::eInfDev:
        return "Infdev";
    case VersionRelease::eRelease:
        return "Release";
    case VersionRelease::eStaging:
        return "Staging";
    case VersionRelease::eSnapshot:
        return "Snapshot";
    default:
        return "Undefined";
    }
}

struct Version {
    Version()
        : release(VersionRelease::eAlpha)
        , major(0)
        , minor(1)
        , patch(0)
    {
    }

    Version(VersionRelease release, uint32_t major, uint32_t minor, uint32_t patch)
        : release(release)
        , major(major)
        , minor(minor)
        , patch(patch)
    {
    }

    VersionRelease release;
    uint32_t major;
    uint32_t minor;
    uint32_t patch;

    std::string ToString() const
    {
        return fmt::format("{} {}.{}.{}", bl::to_string(release), major, minor, patch);
    }
};

const Version engineVersion(VersionRelease::eAlpha, 0, 2, 0);
const std::string engineName = "Bluemetal Engine";

} // namespace bl

