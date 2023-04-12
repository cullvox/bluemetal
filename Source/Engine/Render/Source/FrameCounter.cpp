#include "Core/Log.hpp"
#include "Render/FrameCounter.hpp"

void blFrameCounter::beginFrame() noexcept
{
    _startOfFrame = std::chrono::high_resolution_clock::now();
    _frameCount++;
}

bool blFrameCounter::endFrame() noexcept
{
    bool endedSecond{};

    _endOfFrame = std::chrono::high_resolution_clock::now();

    // Compute the frames per second if one second has passed.
    if (_endOfFrame - _lastSecond > std::chrono::seconds(1))
    {
        
        // Keep within the alloted size of the average frames held.
        if (_framesPerSecond.size() == _maximumHeldFramesPerSecond)
        {
            _framesPerSecond.pop_front();
        }
        
        // Add this latest frame.
        _framesPerSecond.push_back(_frameCount);

        // Reset the frame counter.
        _frameCount = 0;

        // Set the last second to this now.
        _lastSecond = _endOfFrame;

        // Set the ended second value to true.
        endedSecond = true;
    }

    // Compute the milliseconds per frame.
    auto ms = std::chrono::duration_cast<
                std::chrono::milliseconds>(_endOfFrame - _startOfFrame);

    // Pop the front if the maximum will be held
    if (_millisecondsPerFrame.size() == _maximumHeldMillisecondsPerFrame)
    {
        _millisecondsPerFrame.pop_front();
    }

    // Add the ms to the frames
    _millisecondsPerFrame.push_back(ms);

    return endedSecond;
}

int blFrameCounter::getFramesPerSecond() const noexcept
{
    return _framesPerSecond.back();
}

float blFrameCounter::getAverageFramesPerSecond(int seconds) const noexcept
{

    // Make sure we are getting a proper second count.
    if (seconds > _maximumHeldFramesPerSecond || seconds < 1)
    {
        Logger::Error("Attempting to get an average FPS greater than the "
            "maximum at {} seconds.", seconds);
    }

    // Get the average fps
    seconds = std::clamp(seconds, 1, (int)_framesPerSecond.size());

    float averageFPS = 0;
    for (int fps : _framesPerSecond)
        averageFPS += fps;
    averageFPS /= seconds;

    return averageFPS;
}

float blFrameCounter::getMillisecondsPerFrame() const noexcept
{
    const auto msPerFrame = _millisecondsPerFrame.back();

    // Overly convoluted way to convert from milli to float seconds.
    return std::chrono::duration_cast<
        std::chrono::duration<float, std::milli>>(msPerFrame).count();
}

float blFrameCounter::getAverageMillisecondsPerFrame(int frames) const noexcept
{
    
    // Make sure we are getting a proper second count
    if (frames > _maximumHeldMillisecondsPerFrame || frames < 1)
    {
        Logger::Warning("Attempting to get an average milliseconds per frame "
        "greater than the maximum frame count at {} frames.", frames);
    }

    frames = std::clamp(frames, 1, (int)_millisecondsPerFrame.size());

    // Add all the frames together and divide them by the seconds for an average.
    float averageMPF = 0;
    for (auto ms : _millisecondsPerFrame)
    {
        averageMPF += std::chrono::duration_cast<
            std::chrono::duration<float, std::milli>>(ms).count();
    }
    averageMPF /= frames;
    return averageMPF;
}