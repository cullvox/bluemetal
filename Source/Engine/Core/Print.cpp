#include "Print.h"

namespace bl
{


void Log::EnableVerboseLogging(bool enable)
{
    useVerboseLogging = enable;
}

bool Log::IsVerboseLogging()
{
    return useVerboseLogging;
}

}
