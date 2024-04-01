#pragma once

#include "Platform.h"

namespace bl {
namespace PathUtils {

static constexpr const char* GetFilename(const char* path) {

#ifdef BLUEMETAL_SYSTEM_WINDOWS
    const char Slash = '\\';
#else
    const char Slash = '/';
#endif

    const char* file = path;
    while (*path) {
        if (*path++ == Slash) {
            file = path;
        }
    }
    return file;
}

} // namespace PathUtils
} // namespace bl
