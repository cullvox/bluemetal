#pragma once

#include "Render/RenderDevice.hpp"
#include "Render/Image.hpp"
#include "Render/Swapchain.hpp"
#include "Render/RenderPass.hpp"
#include "Render/Pipeline.hpp"

struct Submission
{
    blPipeline* pipeline;
    blBuffer* indexBuffer;
    blBuffer* vertexBuffer;
    int instanceCount;
};

class BLOODLUST_RENDER_API blRenderer
{
    static const inline uint32_t DEFAULT_FRAMES_IN_FLIGHT = 2;
public:
    blRenderer(std::shared_ptr<const blRenderDevice> renderDevice,
        std::shared_ptr<const blSwapchain> swapchain,
        const std::vector<std::shared_ptr<blRenderPass>>& passes);
    ~blRenderer();

    void resize(blExtent2D extent);
    void submit(const Submission& submission) noexcept;
    void render();

private:
    void buildSyncObjects();
    void collapseSyncObjects() noexcept;

    uint32_t _currentFrame;
    std::vector<vk::CommandBuffer> _swapCommandBuffers;
    std::vector<vk::Semaphore> _imageAvailableSemaphores;
    std::vector<vk::Semaphore> _renderFinishedSemaphores;
    std::vector<vk::Fence> _inFlightFences;
    std::shared_ptr<const blRenderDevice> _renderDevice;
    std::shared_ptr<const blSwapchain> _swapchain;
    const std::vector<std::shared_ptr<blRenderPass>> _passes;  
};