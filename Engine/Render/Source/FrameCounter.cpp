#include "Render/FrameCounter.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>

namespace bl
{

FrameCounter::FrameCounter() noexcept
{
}

FrameCounter::~FrameCounter() noexcept
{
}

void FrameCounter::beginFrame()
{
    // Get start time.
    m_startOfFrame = std::chrono::high_resolution_clock::now();
    m_frameCount++;
}

bool FrameCounter::endFrame()
{
    bool endedSecond{};

    // Get end time.
    m_endOfFrame = std::chrono::high_resolution_clock::now();

    // Compute the frames per second if one second has passed.
    if (m_endOfFrame - m_lastSecond > std::chrono::seconds(1))
    {
        // Pop the front if the size is greater than the maximum.
        if (m_framesPerSecond.size() == m_maximumHeldFramesPerSecond)
        {
            m_framesPerSecond.pop_front();
        }
        
        
        // Add this latest frame.
        m_framesPerSecond.push_back(m_frameCount);

        // Reset the frame counter.
        m_frameCount = 0;

        // Set the last second to this now.
        m_lastSecond = m_endOfFrame;

        // Set the ended second value to true.
        endedSecond = true;
    }

    // Compute the milliseconds per frame.
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(m_endOfFrame - m_startOfFrame);

    // Pop the front if the maximum will be held
    if (m_millisecondsPerFrame.size() == m_maximumHeldMillisecondsPerFrame)
    {
        m_millisecondsPerFrame.pop_front();
    }

    // Add the ms to the frames
    m_millisecondsPerFrame.push_back(ms);

    return endedSecond;
}

int FrameCounter::getFramesPerSecond() const noexcept
{
    return m_framesPerSecond.back();
}

float FrameCounter::getAverageFramesPerSecond(int seconds) const noexcept
{

    // Make sure we are getting a proper second count.
    if (seconds > m_maximumHeldFramesPerSecond || seconds < 1)
    {
        spdlog::warn("Attempting to get an average FPS greater than the maximum at {} seconds.", seconds);
    }

    // Clamp as a precaution
    seconds = std::clamp(seconds, 1, (int)m_framesPerSecond.size());

    // Add all the seconds together and divide them by the seconds for an average.
    float averageFPS = 0;
    std::for_each_n(m_framesPerSecond.begin(), seconds, [&](int fps){
        averageFPS += fps;
    });
    averageFPS /= seconds;

    return averageFPS;
}

float FrameCounter::getMillisecondsPerFrame() const noexcept
{
    const auto msPerFrame = m_millisecondsPerFrame.back();
    return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(msPerFrame).count();
}

float FrameCounter::getAverageMillisecondsPerFrame(int frames) const noexcept
{
    
    // Make sure we are getting a proper second count
    if (frames > m_maximumHeldMillisecondsPerFrame || frames < 1)
    {
        spdlog::warn("Attempting to get an average milliseconds per frame greater than the maximum frame count at {} frames.", frames);
    }

    frames = std::clamp(frames, 1, (int)m_millisecondsPerFrame.size());

    // Add all the frames together and divide them by the seconds for an average.
    float averageMPF = 0;
    std::for_each_n(m_millisecondsPerFrame.begin(), frames, [&](std::chrono::milliseconds ms){
        averageMPF += std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(ms).count();
    });
    averageMPF /= frames;
    return averageMPF;
}

} // namespace bl