#pragma once

#include "Export.h"
#include "Precompiled.h"

namespace bl {

class BLUEMETAL_API FrameCounter {
public:
    FrameCounter(int maxFramesCounted);
    ~FrameCounter();

    void beginFrame();
    bool endFrame(); // returns true if this frame ended at frame count.
    int getFramesPerSecond();
    float getAverageFramesPerSecond(uint32_t seconds);
    float getMillisecondsPerFrame();
    float getAverageMillisecondsPerFrame(uint32_t frames);

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

} // namespace bl
