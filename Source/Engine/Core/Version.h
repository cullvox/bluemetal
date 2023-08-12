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

const Version applicationVersion(VersionRelease::eAlpha, 0, 1, 0);
const std::string applicationName = "Maginvox";
const Version engineVersion(VersionRelease::eAlpha, 0, 1, 0);
const std::string engineName = "Bluemetal Engine";

} // namespace bl