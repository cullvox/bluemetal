#pragma once

#include "Precompiled.h"

namespace bl {

class FrameCounter {
public:
    FrameCounter(int maxFramesCounted = 144);
    ~FrameCounter();

    void BeginFrame();
    bool EndFrame(); // returns true if this frame ended at frame count.
    int GetFramesPerSecond();
    float GetAverageFramesPerSecond(uint32_t seconds);
    float GetMillisecondsPerFrame();
    float GetAverageMillisecondsPerFrame(uint32_t frames);
    float GetDeltaTime() const;

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
    float                                _delta;
};

} // namespace bl
