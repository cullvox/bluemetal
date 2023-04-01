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
class BLOODLUST_RENDER_API Renderer
{
public:

    Renderer() noexcept;
    Renderer(RenderDevice& renderDevice, Swapchain& swapchain) noexcept;
    ~Renderer();

    Renderer& operator=(Renderer&& rhs) noexcept;
    Renderer& operator=(Renderer&) noexcept = delete;

    bool good() const noexcept;
    bool recreate() noexcept; // Renderer requires resizing of some of its internals, resizes swapchain too.
    bool beginFrame(VkCommandBuffer& commandBuffer) noexcept;
    bool endFrame() noexcept;

    VkRenderPass getRenderPass() const noexcept;

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
    /// @return True on success, false on failure.
    bool createCommandBuffers() noexcept;

    /// @brief Creates the frame buffers used to render the swap images to present.
    /// @return True on success, false on failure.
    bool createFrameBuffers() noexcept;

    /// @brief Creates the synchronization tools when swapping between images on the GPU/CPU.
    /// @return True on success, false on failure.
    bool createSyncObjects() noexcept;

    /// @brief Destroys the swap objects.
    void destroySwappable() noexcept;

    void collapse() noexcept;

    // TODO: Create a swap renderer class of some kind for the swapped obejects per frame.
    // This will probably have to include synchronized objects.
    
    RenderDevice* _pRenderDevice;
    Swapchain* _pSwapchain;
    VkFormat _depthFormat;
    Image _depthImage;
    VkRenderPass _pass;
    uint32_t _currentFrame;
    uint32_t _previousFrame;
    uint32_t _imageIndex;
    bool _framebufferResized;
    bool _deadFrame;
    std::vector<VkCommandBuffer> _swapCommandBuffers;
    std::vector<VkImageView> _swapImageViews;
    std::vector<VkFramebuffer> _swapFramebuffers;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    
};

} /* namespace bl */