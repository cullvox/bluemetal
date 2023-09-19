#pragma once

namespace bl {
namespace PathUtils {

#ifdef BLUEMETAL_SYSTEM_WINDOW
    const char Slash = '\\';
#else
    const char Slash = '/';
#endif

static constexpr const char* GetFilename(const char* path) {
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
