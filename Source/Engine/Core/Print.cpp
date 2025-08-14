#include "Print.h"

namespace bl
{

bool Print::useVerboseLogging;

void Print::EnableVerboseLogging(bool enable)
{
    useVerboseLogging = enable;
}

bool Print::IsVerboseLogging()
{
    return useVerboseLogging;
}

}
