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

class Renderer
{
public:
    static const inline uint32_t DEFAULT_FRAMES_IN_FLIGHT = 2; 

    Renderer(Window& window, RenderDevice& renderDevice, Swapchain& swapchain);
    ~Renderer();

    void submit(const Submission& submission);
    void displayFrame();
private:
    void getDepthFormat();
    void createRenderPass();
    void createCommandBuffers();
    void createFrameBuffers();
    void createSyncObjects();
    void recreateSwappable(bool destroy = true);
    void destroySwappable();
    void beginRender();
    void endRender();

    Window& m_window;
    RenderDevice& m_renderDevice;
    Swapchain& m_swapchain;
    VkFormat m_depthFormat;
    Image m_depthImage;
    VkRenderPass m_pass;
    uint32_t m_currentFrame;
    uint32_t m_imageIndex;
    bool m_framebufferResized;
    bool m_firstFrame;
    std::vector<VkCommandBuffer> m_swapCommandBuffers;
    std::vector<VkImageView> m_swapImageViews;
    std::vector<VkFramebuffer> m_swapFramebuffers;
    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    
};

} /* namespace bl */