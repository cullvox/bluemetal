#pragma once

#include "Core/Export.h"

#include <list>
#include <chrono>

namespace bl
{

// A Simple class which calculates frame rates and other useful numbers.
class BLOODLUST_API FrameCounter
{
public:
    FrameCounter() noexcept;
    ~FrameCounter() noexcept;

    void beginFrame();
    bool endFrame(); // returns true if this frame ended with a frame count.
    int getFramesPerSecond() const noexcept;
    float getAverageFramesPerSecond(int seconds) const noexcept;
    float getMillisecondsPerFrame() const noexcept;
    float getAverageMillisecondsPerFrame(int frames) const noexcept;

private:
    int m_frameCount = 0;
    std::chrono::high_resolution_clock::time_point m_startOfFrame;
    std::chrono::high_resolution_clock::time_point m_endOfFrame;
    std::chrono::high_resolution_clock::time_point m_lastSecond;
    int m_maximumHeldFramesPerSecond = 120;
    int m_maximumHeldMillisecondsPerFrame = 144;
    std::list<int> m_framesPerSecond;
    std::list<std::chrono::milliseconds> m_millisecondsPerFrame;
};

} // namespace bl