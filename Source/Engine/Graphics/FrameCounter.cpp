#include "FrameCounter.h"
#include "Core/Print.h"

namespace bl {

FrameCounter::FrameCounter(int maxFramesCounted)
    : _maximumHeldFramesPerSecond(maxFramesCounted)
{
}

FrameCounter::~FrameCounter()
{
}

void FrameCounter::beginFrame()
{
    _startOfFrame = std::chrono::high_resolution_clock::now();
    _frameCount++;
}

bool FrameCounter::endFrame()
{
    bool endedSecond {};

    _endOfFrame = std::chrono::high_resolution_clock::now();

    // Compute the frames per second if one second has passed.
    if (_endOfFrame - _lastSecond > std::chrono::seconds(1)) {

        // Keep within the alloted size of the average frames held.
        if (_framesPerSecond.size() == _maximumHeldFramesPerSecond) {
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

    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(_endOfFrame - _startOfFrame);

    // Pop the front if the maximum will be held
    if (_millisecondsPerFrame.size() == _maximumHeldMillisecondsPerFrame) {
        _millisecondsPerFrame.pop_front();
    }

    // Add the ms to the frames
    _millisecondsPerFrame.push_back(ns);

    return endedSecond;
}

int FrameCounter::getFramesPerSecond()
{
    if (_framesPerSecond.size() < 1)
        return 0;
    return _framesPerSecond.back();
}

float FrameCounter::getAverageFramesPerSecond(uint32_t seconds)
{

    // Make sure we are getting a proper second count.
    if (seconds > _maximumHeldFramesPerSecond || seconds < 1) {
        blError(
            "Attempting to get an average FPS greater than the maximum at {} seconds.", seconds);
    }

    // Get the average fps
    seconds = std::clamp(seconds, (uint32_t)0, (uint32_t)_framesPerSecond.size());

    float averageFPS = 0;

    std::for_each_n(_framesPerSecond.rbegin(), seconds, [&](int fps) { averageFPS += fps; });

    averageFPS /= seconds;

    return averageFPS;
}

float FrameCounter::getMillisecondsPerFrame()
{
    if (_millisecondsPerFrame.size() < 1)
        return 0.0f;
    const auto msPerFrame = _millisecondsPerFrame.back();

    // Overly convoluted way to convert from nano to float milliseconds.
    return std::chrono::duration_cast<std::chrono::duration<float, std::nano>>(msPerFrame).count()
        / 1000000.0f;
}

float FrameCounter::getAverageMillisecondsPerFrame(uint32_t frames)
{

    // Make sure we are getting a proper second count
    if (frames > _maximumHeldMillisecondsPerFrame || frames < 1) {
        blError(
            "Attempting to get an average milliseconds per frame greater than the maximum frame "
            "count at {} frames.",
            frames);
    }

    frames = std::clamp(frames, (uint32_t)0, (uint32_t)_millisecondsPerFrame.size());

    // Add all the frames together and divide them by the seconds for an average.
    float averageMPF = 0;

    std::for_each_n(_millisecondsPerFrame.rbegin(), frames, [&](std::chrono::nanoseconds ms) {
        averageMPF
            += std::chrono::duration_cast<std::chrono::duration<float, std::nano>>(ms).count()
            / 1000000.0f;
    });

    averageMPF /= frames;
    return averageMPF;
}

} // namespace bl
