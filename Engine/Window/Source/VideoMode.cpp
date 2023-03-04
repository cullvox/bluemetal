#include "Window/VideoMode.hpp"

namespace bl {

VideoMode::VideoMode(Extent2D resolution, unsigned int bitsPerPixel, unsigned int refreshRate)
    : resolution(resolution), bitsPerPixel(bitsPerPixel), refreshRate(refreshRate)
{
}

} /* namespace bl */