#pragma once

#include "Core/Precompiled.hpp"
#include "Render/Export.h"

class BLUEMETAL_RENDER_API blFrameCounter
{
public:
    void beginFrame() noexcept;
    bool endFrame() noexcept; // returns true if this frame ended with a frame count.
    int getFramesPerSecond() const noexcept;
    float getAverageFramesPerSecond(int seconds) const noexcept;
    float getMillisecondsPerFrame() const noexcept;
    float getAverageMillisecondsPerFrame(int frames) const noexcept;

private:
    using time_point = std::chrono::high_resolution_clock::time_point;

    int                                  _frameCount = 0;
    time_point                           _startOfFrame;
    time_point                           _endOfFrame;
    time_point                           _lastSecond;
    int                                  _maximumHeldFramesPerSecond = 120;
    int                                  _maximumHeldMillisecondsPerFrame = 144;
    std::list<int>                       _framesPerSecond;
    std::list<std::chrono::nanoseconds>  _millisecondsPerFrame;
};