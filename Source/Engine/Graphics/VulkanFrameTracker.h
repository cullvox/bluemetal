#pragma once

#include "VulkanConfig.h"

namespace bl {

class VulkanFrameTracker {
public:
    VulkanFrameTracker(uint32_t startFrame = 0)
        : _currentFrame(startFrame) {}
    ~VulkanFrameTracker();

    void Next() {
        _currentFrame = (_currentFrame + 1) % VulkanConfig::numFramesInFlight;
    }

    uint32_t PeekNext() const {
        return (_currentFrame + 1) % VulkanConfig::numFramesInFlight;
    }

    uint32_t Prev() const {
        return (_currentFrame - 1) % VulkanConfig::numFramesInFlight;
    }

    uint32_t Get() const {
        return _currentFrame;
    }

private:
    uint32_t _currentFrame;
};

} // namespace bl