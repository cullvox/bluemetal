#include "Print.h"

namespace bl
{

bool Log::useVerboseLogging;

void Log::EnableVerboseLogging(bool enable)
{
    useVerboseLogging = enable;
}

bool Log::IsVerboseLogging()
{
    return useVerboseLogging;
}

}
