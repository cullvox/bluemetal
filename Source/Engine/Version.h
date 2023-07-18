#pragma once

#include "Export.h"
#include "Precompiled.h"

enum class blVersionRelease : uint32_t
{
    eAlpha,
    eBeta,
    eInfiniteDevelopment,
    eRelease,
    eStaging,
    eSnapshot,
    eUndefined,
};

struct BLUEMETAL_API blVersion
{
    blVersionRelease release;
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

const blVersion applicationVersion(blVersionRelease::eAlpha, 0, 1, 0);
const std::string applicationName = "Maginvox";
const blVersion engineVersion(blVersionRelease::eAlpha, 0, 1, 0);
const std::string engineName = "Bluemetal Engine";