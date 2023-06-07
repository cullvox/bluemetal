#pragma once

#include "Core/Precompiled.hpp"
#include "Render/Export.h"
#include <cstdint>

class BLUEMETAL_RENDER_API blFrameCounter
{
public:
    void beginFrame() noexcept;
    bool endFrame() noexcept; // returns true if this frame ended with a frame count.
    int getFramesPerSecond() const noexcept;
    float getAverageFramesPerSecond(uint32_t seconds) const noexcept;
    float getMillisecondsPerFrame() const noexcept;
    float getAverageMillisecondsPerFrame(uint32_t frames) const noexcept;

private:
    using time_point = std::chrono::high_resolution_clock::time_point;

    uint32_t                             _frameCount = 0;
    time_point                           _startOfFrame;
    time_point                           _endOfFrame;
    time_point                           _lastSecond;
    uint32_t                             _maximumHeldFramesPerSecond = 120;
    uint32_t                             _maximumHeldMillisecondsPerFrame = 144;
    std::list<uint32_t>                  _framesPerSecond;
    std::list<std::chrono::nanoseconds>  _millisecondsPerFrame;
};