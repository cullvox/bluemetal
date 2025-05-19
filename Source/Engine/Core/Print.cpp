#include "Print.h"

namespace bl
{

namespace logging
{

static bool useVerboseLogging;

void enableVerboseLogging(bool enable)
{
    useVerboseLogging = enable;
}

bool isVerboseLogging()
{
    return useVerboseLogging;
}

}

}
