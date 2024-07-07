#pragma once

#include "Precompiled.h"
#include "Vulkan.h"
#include "VulkanConfig.h"
#include "VulkanFrameTracker.h"

namespace bl {

/// @brief Deleter function used when deleting a Vulkan object.
using VulkanDeleterFunction = std::function<void(void)>;

/// @brief Deletes Vulkan objects after the last time it's been used in the currently recorded frames.
class VulkanDeleterQueue {
public:

    /// @brief Constructor
    VulkanDeleterQueue();

    /// @brief Destructor
    ~VulkanDeleterQueue();

    void PushDeleter(VulkanDeleterFunction func) {
        _queues[_frameTracker.Get()].push(func);
    }

    void Run() {
        // Increment first, this is because we always are deleting the frame in front.
        _frameTracker.Next();

        auto& queue = _queues[_frameTracker.Get()];
        
        // Run the deleter functions and pop until the queue is empty.
        for (; not queue.empty(); queue.pop())
            queue.front()();
    }

private:
    VulkanFrameTracker _frameTracker;
    std::array<std::queue<VulkanDeleterFunction>, VulkanConfig::numFramesInFlight> _queues;
};

} // namespace bl