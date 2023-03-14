#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Image.hpp"
#include "Render/Swapchain.hpp"

namespace bl {


struct Pipeline;
struct Buffer;

struct Submission
{
    Pipeline& pipeline;
    Buffer& indexBuffer;
    Buffer& vertexBuffer;
    int instanceCount;
};

/// @brief Sets up sync, render textures, command buffers, and render passes for submissions.
class BLOODLUST_API Renderer
{
public:

    Renderer() noexcept;
    Renderer(RenderDevice& renderDevice, Swapchain& swapchain) noexcept;
    
    ~Renderer();

    Renderer& operator=(Renderer&& rhs) noexcept;

    bool beginFrame() noexcept;
    bool endFrame() noexcept;

    void submit(const Submission& submission) noexcept;

private:
    static const inline uint32_t DEFAULT_FRAMES_IN_FLIGHT = 2;

    /// @brief Determines hte image format for the depth buffer.
    /// @return The depth format.
    VkFormat getDepthFormat() noexcept;

    /// @brief Creates the render pass vulkan object.
    /// @return True on success, false on failure.
    bool createRenderPass() noexcept;

    /// @brief Creates the command buffers used when rendering.
    /// @return True on succes, false on failure.
    bool createCommandBuffers() noexcept;

    /// @brief Creates the frame buffers used to render the swap images to present.
    /// @return True on success, false on failure.
    bool createFrameBuffers() noexcept;

    /// @brief Creates the syncronization tools when swapping between images on the GPU/CPU.
    /// @return True on success, false on failure.
    bool createSyncObjects() noexcept;

    /// @brief Short hand for creating the command buffers, frame buffers and sync objects.
    /// @return True on succcess, false on failure.
    bool recreateSwappable() noexcept;

    /// @brief Destroys the swap objects.
    void destroySwappable() noexcept;

    void collapse() noexcept;

    // TODO: Create a swap renderer class of some kind for the swapped obejects per frame.
    // This will probably have to include synchronized objects.
    
    RenderDevice* m_pRenderDevice;
    Swapchain* m_pSwapchain;
    VkFormat m_depthFormat;
    Image m_depthImage;
    VkRenderPass m_pass;
    uint32_t m_currentFrame;
    uint32_t m_previousFrame;
    uint32_t m_imageIndex;
    bool m_framebufferResized;
    bool m_deadFrame;
    std::vector<VkCommandBuffer> m_swapCommandBuffers;
    std::vector<VkImageView> m_swapImageViews;
    std::vector<VkFramebuffer> m_swapFramebuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    
};

} /* namespace bl */